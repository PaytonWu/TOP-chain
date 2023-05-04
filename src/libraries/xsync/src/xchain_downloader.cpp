// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xsync/xchain_downloader.h"

#include "xmbus/xevent_executor.h"
#include "xmetrics/xmetrics.h"
#include "xsync/xsync_log.h"
#include "xsync/xsync_util.h"
#include "xdata/xnative_contract_address.h"
#include "xdata/xgenesis_data.h"
#include "xpbase/base/top_utils.h"
#include "xsync/xsync_message.h"
#include "xdata/xfull_tableblock.h"
#include "xdata/xtable_bstate.h"
#include "xsync/xsync_store_shadow.h"
#include "xsync/xsync_pusher.h"
#include "xstatestore/xstatestore_face.h"

NS_BEG2(top, sync)

using namespace mbus;
using namespace data;

#define BATCH_SIZE 20
#define BATCH_MAX_END (130)  //130  BATCH_SIZE < BATCH_MAX_END

xchain_downloader_t::xchain_downloader_t(std::string vnode_id,
                                 xsync_store_face_t * sync_store,
                                 xrole_xips_manager_t *role_xips_mgr,
                                 xrole_chains_mgr_t *role_chains_mgr,
                                 const observer_ptr<base::xvcertauth_t> &certauth,
                                 xsync_sender_t *sync_sender,
                                 xsync_ratelimit_face_t *ratelimit,
                                 const std::string & address):
  m_vnode_id(vnode_id),
  m_sync_store(sync_store),
  m_certauth(certauth),
  m_sync_sender(sync_sender),
  m_ratelimit(ratelimit),
  m_address(address),
  m_sync_range_mgr(vnode_id, address),
  m_role_xips_mgr(role_xips_mgr),
  m_role_chains_mgr(role_chains_mgr) {
    xsync_info("chain_downloader create_chain %s", m_address.c_str());
    m_is_elect_chain = is_elect_chain();
}


uint64_t xchain_downloader_t::get_chain_last_end_height(enum_chain_sync_policy sync_policy)
{
    return m_chain_objects[sync_policy].get_end_height();
}

xchain_downloader_t::~xchain_downloader_t() {

    xsync_info("chain_downloader destroy %s", m_address.c_str());
}

const std::string& xchain_downloader_t::get_address() const {
    return m_address;
}

void xchain_downloader_t::on_role_changed(const xchain_info_t & chain_info) {
    enum_role_changed_result ret = m_sync_range_mgr.on_role_changed(chain_info);
    if (ret == enum_role_changed_result_none) {
        return;
    }

    xsync_dbg("chain_downloader on_role_changed %s", m_address.c_str());
}


void xchain_downloader_t::destroy() {
    clear();
}

bool xchain_downloader_t::downloading(int64_t now) {
    if (!m_task.finished()){
        if (!m_task.expired()) {
            return true;
        }
        m_continuous_times++;
    }

    return false;
}

bool xchain_downloader_t::on_timer(int64_t now) {
    std::pair<uint64_t, uint64_t> interval;
    vnetwork::xvnode_address_t self_addr;
    vnetwork::xvnode_address_t target_addr;

    for (uint32_t index = (m_current_object_index + 1) % enum_chain_sync_policy_max, count = 0; count < enum_chain_sync_policy_max;
            count++, index = (index + 1) % enum_chain_sync_policy_max){

        if (index == enum_chain_sync_policy_full || index == enum_chain_sync_policy_checkpoint) {
            if (false == m_chain_objects[index].check_and_fix_behind_height(now, m_sync_store, index, m_address)) {
                continue;
            }
        }

        if (!m_chain_objects[index].pick(interval, self_addr, target_addr)) {
            continue;
        }

        m_current_object_index = index;
        m_task.start();
        xsync_download_command_t command(interval, (enum_chain_sync_policy)index, self_addr, target_addr);
        xsync_command_execute_result result = m_task.execute(command);
        if (finish == result) {
            m_chain_objects[index].set_height(m_chain_objects[index].picked_height() + 1);
            m_continuous_times = 0;
        } else if (abort == result) {
            m_continuous_times++;
        }

        if (m_continuous_times >= 5){
            for (uint32_t i = 0; i < enum_chain_sync_policy_max; i++) {
                m_chain_objects[i].clear();
            }

            m_continuous_times = 0;
        }

        if (abort_overflow == result){
            xsync_warn("chain_downloader execute net error,slef_address:%s target_address:%s.", m_address.c_str(), target_addr.to_string().c_str());
            return false;
        }
        break;
    }

    return true;
}

void xchain_downloader_t::on_response(std::vector<data::xblock_ptr_t> &blocks, const vnetwork::xvnode_address_t &self_addr, const vnetwork::xvnode_address_t &from_addr) {
    xsync_on_blocks_response_command_t command(blocks, self_addr, from_addr);
    xsync_info("chain_downloader on_response %s count(%u) %s, %d",
        m_address.c_str(), blocks.size(), from_addr.to_string().c_str(), m_task.finished());
    if ((!m_task.finished()) && (!m_task.expired())) {
        xsync_command_execute_result result = m_task.execute(command);
        if (finish == result) {
            m_chain_objects[m_current_object_index].set_height(m_chain_objects[m_current_object_index].picked_height() + 1);
            m_continuous_times = 0;
        } else if (abort == result) {
            m_continuous_times++;
        } else {
             m_continuous_times = 0;
        }
    }
}
enum_result_code xchain_downloader_t::handle_archive_block(xblock_ptr_t &block, uint64_t quota_height) {
    if (m_is_elect_chain) {
        if (!check_auth(m_certauth, block)) {
            xsync_dbg("xchain_downloader_t::handle_block, check_auth fail.");
            return enum_result_code::auth_failed;
        }
    }

    auto vbindex = m_sync_store->load_block_object(block->get_block_owner(), block->get_height(), false, block->get_viewid());
    if (vbindex == nullptr) {
    //XTODO,need doublecheck whether allow set flag of authenticated without verify signature
        xsync_dbg("xchain_downloader_t::handle_block, store_block: %s,%d", block->get_account().c_str(), block->get_height());
        block->set_block_flag(enum_xvblock_flag_authenticated);
        
        base::xvblock_t* vblock = dynamic_cast<base::xvblock_t*>(block.get());
        bool ret = m_sync_store->store_block(vblock);
        if (!ret) {
            return enum_result_code::failed;
        }
    } else {
    }

    return enum_result_code::success;
}
bool xchain_downloader_t::is_elect_chain() {
    bool is_elect_chain = false;
    std::string account_prefix;
    uint32_t table_id = 0;
    data::xdatautil::extract_parts(m_address, account_prefix, table_id);

    if (account_prefix == common::rec_table_base_address.to_string() || account_prefix == common::zec_table_base_address.to_string() ||
        account_prefix == common::relay_table_base_address.to_string()) {
        is_elect_chain = true;
    }
    return is_elect_chain;
}

void xchain_downloader_t::on_archive_blocks(std::vector<data::xblock_ptr_t> &blocks, const vnetwork::xvnode_address_t &self_addr, const vnetwork::xvnode_address_t &from_addr) {
    uint32_t count = blocks.size();
    if (count == 0) {
        return;
    }

    xsync_info("chain_downloader on_archive_blocks, %s count(%u) %s",
        m_address.c_str(), count, from_addr.to_string().c_str());

    if (!m_is_elect_chain){
        xblock_ptr_t &block = blocks[count-1];
        if (!check_auth(m_certauth, block)) {
            xsync_info("chain_downloader on_archive_blocks(auth_failed) %s,height=%lu,", m_address.c_str(), block->get_height());
            return;
        }
    }

    auto next_block = blocks[blocks.size() - 1];
 
    for (uint32_t i = 0; i < count; i++) {
        xblock_ptr_t &block = blocks[i];
        enum_result_code ret = handle_archive_block(block, next_block->get_height());

        if (ret == enum_result_code::success) {
            xsync_dbg("chain_downloader on_archive_blocks(succ) %s,height=%lu,viewid=%lu,prev_hash:%s,",
                m_address.c_str(), block->get_height(), block->get_viewid(), to_hex_str(block->get_last_block_hash()).c_str());
        } else if (ret == enum_result_code::failed) {
            xsync_warn("chain_downloader on_archive_blocks(failed) reason %d, block is: %s", ret, block->dump().c_str());
        }
    }

    xsync_info("chain_downloader on_archive_blocks(total) %s,current(height=%lu,viewid=%lu,hash=%s) behind(height=%lu)",
        m_address.c_str(), next_block->get_height(), next_block->get_viewid(), to_hex_str(next_block->get_block_hash()).c_str(), m_sync_range_mgr.get_behind_height());

}

void xchain_downloader_t::on_behind(uint64_t start_height, uint64_t end_height, enum_chain_sync_policy sync_policy, const vnetwork::xvnode_address_t &self_addr, const vnetwork::xvnode_address_t &target_addr, const std::string &reason) {
    if ((start_height > end_height) || end_height == 0) {
        return;
    }

    uint64_t height = m_chain_objects[sync_policy].height();
    m_chain_objects[sync_policy].set_object_t(start_height, end_height, self_addr, target_addr);
    if(start_height > height) {
        m_chain_objects[sync_policy].set_height(start_height);
    }

    xsync_info("chain_downloader on_behind expect start_height=%lu,height=%llu, end_height=%llu, target address %s, sync policy %d, chain is %s, reason:%s ",
                start_height, height, end_height, target_addr.to_string().c_str(), sync_policy, m_address.c_str(), reason.c_str());
}

void xchain_downloader_t::on_block_committed_event(uint64_t height) {
    xsync_on_commit_event_command_t command(height);
    if ((!m_task.finished()) && (!m_task.expired())) {
        xsync_command_execute_result result = m_task.execute(command);
        if (finish == result) {
            m_chain_objects[m_current_object_index].set_height(m_chain_objects[m_current_object_index].picked_height() + 1);
            m_continuous_times = 0;
        } else if (abort == result) {
            m_continuous_times++;
        } else {
            m_continuous_times = 0;
        }
    }
}



enum_result_code xchain_downloader_t::handle_block(xblock_ptr_t &block, uint64_t quota_height) {
    if (m_is_elect_chain) {
        if (!check_auth(m_certauth, block)) {
            xsync_dbg("xchain_downloader_t::handle_block, check_auth fail.");
            return enum_result_code::auth_failed;
        }
    }

    //temperary code
    auto vbindex = m_sync_store->load_block_object(block->get_block_owner(), block->get_height(), false, block->get_viewid());
    if (vbindex == nullptr) {
    //XTODO,need doublecheck whether allow set flag of authenticated without verify signature
        xsync_dbg("xchain_downloader_t::handle_block, store_block: %s,%d", block->get_account().c_str(), block->get_height());
        block->set_block_flag(enum_xvblock_flag_authenticated);
        
        base::xvblock_t* vblock = dynamic_cast<base::xvblock_t*>(block.get());
        wait_committed_event_group(block->get_height(), quota_height);
        bool ret = m_sync_store->store_block(vblock);
        if (!ret) {
            return enum_result_code::failed;
        }
    } else {
        xsync_dbg("xchain_downloader_t::handle_block, update: %s,%d", block->get_account().c_str(), block->get_height());
        if (vbindex->check_block_flag(enum_xvblock_flag_committed)) {
            m_sync_store->get_shadow()->on_chain_event(block->get_block_owner(), block->get_height());
        } else {
            wait_committed_event_group(block->get_height(), quota_height);
        }
    }

    return enum_result_code::success;
}

int64_t xchain_downloader_t::get_time() {
    return base::xtime_utl::gmttime_ms();
}

bool xchain_downloader_t::check_behind(uint64_t height, const char *elect_address) {
    //to be deleted
    uint64_t blk_commit_height = m_sync_store->get_latest_committed_block_height(elect_address);
    uint64_t blk_connect_height = m_sync_store->get_latest_connected_block_height(elect_address);
    if (blk_connect_height < blk_commit_height) {
        xsync_info("chain_downloader on_behind(wait_auth_chain) %s,height=%lu,%s(%lu,%lu)",
                m_address.c_str(), height, elect_address, blk_connect_height, blk_commit_height);
        return false;
    }

    return true;
}

bool xchain_downloader_t::send_request(int64_t now) {
    if (!m_ratelimit->get_token(now)) {
        xsync_dbg("chain_downloader get token failed. %s", m_address.c_str());
        XMETRICS_GAUGE(metrics::xsync_downloader_overflow, 1);
        return false;
    }
    
    vnetwork::xvnode_address_t self_addr = m_request->self_addr;
    common::xnode_type_t node_type = self_addr.type();
    if (!common::has<common::xnode_type_t::fullnode>(node_type)) { // not fullnode
        XMETRICS_GAUGE(metrics::xsync_downloader_request, 1);
        m_request->send_time = now;
        int64_t queue_cost = m_request->send_time - m_request->create_time;
        xsync_info("chain_downloader send sync request(block). %s,range[%lu,%lu] get_token_cost(%ldms) %s",
                   m_request->owner.c_str(),
                   m_request->start_height,
                   m_request->start_height + m_request->count - 1,
                   queue_cost,
                   m_request->target_addr.to_string().c_str());
        return m_sync_sender->send_get_blocks(m_request->owner, m_request->start_height, m_request->count, m_request->self_addr, m_request->target_addr);
    }

    std::vector<vnetwork::xvnode_address_t> addresses = m_role_xips_mgr->get_rand_archives(1);
    if (addresses.empty()) {
        xsync_info("xchain_downloader_t::send_request, not find archive.");
        return false;
    }
    XMETRICS_GAUGE(metrics::xsync_downloader_request, 1);
    m_request->send_time = now;
    int64_t queue_cost = m_request->send_time - m_request->create_time;
    vnetwork::xvnode_address_t &target_addr = addresses[0];
    xsync_info("chain_downloader send sync request(block) from fullnode. %s,range[%lu,%lu] get_token_cost(%ldms) %s,%s",
                m_request->owner.c_str(), m_request->start_height, m_request->start_height+m_request->count-1,
                queue_cost, self_addr.to_string().c_str(), target_addr.to_string().c_str());
    return m_sync_sender->send_get_blocks(m_request->owner, m_request->start_height, m_request->count, self_addr, target_addr);
}

xsync_command_execute_result xchain_downloader_t::handle_next(uint64_t current_height) {
    vnetwork::xvnode_address_t self_addr;
    vnetwork::xvnode_address_t target_addr;

    int count_limit = BATCH_SIZE;
    uint64_t start_height = 0;
    uint32_t count = 0;
    uint64_t height = current_height;

    do {
        m_sync_range_mgr.get_next_behind(height, count_limit, start_height, count, self_addr, target_addr);
        if (count == 0) {
            break;
        }
        auto interval = m_sync_store->get_shadow()->get_continuous_unused_interval(m_address, std::make_pair(start_height, start_height + count - 1));
        if (interval.second == 0) {
            height = start_height + count - 1;          
            start_height = 0;
            count = 0;
        } else {
            start_height = interval.first;
            count = interval.second - interval.first + 1;
            m_sync_range_mgr.set_current_sync_start_height(start_height);
            break;
        }
    } while (true);
    
    if (count > 0) {
        int64_t now = get_time();

        xentire_block_request_ptr_t req = create_request(start_height, count);
        if (req != nullptr) {
            req->self_addr = self_addr;
            req->target_addr = target_addr;
            req->create_time = now;
            req->try_time = 0;
            req->send_time = 0;
            m_request = req;
            if (send_request(now)){
                return wait_response;
            } else {
                return abort_overflow;
            }
        }

        return abort;
    }

    return finish;
}


void xchain_downloader_t::clear() {
    m_request = nullptr;
    m_sync_range_mgr.clear_behind_info();
    init_committed_event_group();
}

static void get_elect_info(const xvip2_t & target_group, std::string & elect_address, uint64_t & elect_height) {
    // TODO use macro

    uint8_t zone_id = get_zone_id_from_xip2(target_group);
    uint8_t cluster_id = get_cluster_id_from_xip2(target_group);
    uint8_t group_id = get_group_id_from_xip2(target_group);

    if (zone_id == 0) {
        if (cluster_id == 1) {
            if (group_id >= 1 && group_id < 127)
                elect_address = sys_contract_zec_elect_consensus_addr;
        }
    } else if (zone_id == 1) {
        if (cluster_id == 0 && group_id == 0)
            elect_address = sys_contract_rec_elect_rec_addr;
    } else if (zone_id == 2) {
        if (cluster_id == 0 && group_id == 0)
            elect_address = sys_contract_rec_elect_zec_addr;
    }

    elect_height = get_network_height_from_xip2(target_group);
}

dependency_info_t xchain_downloader_t::get_depend_elect_info(const data::xblock_ptr_t & block) {
    dependency_info_t info;

    const xvip2_t & validator_xip = block->get_cert()->get_validator();
    std::string validator_elect_addr;
    uint64_t validator_elect_height = 0;
    get_elect_info(validator_xip, validator_elect_addr, validator_elect_height);
    info.m_list.push_back(elect_item_t{validator_elect_addr, validator_elect_height});

    const xvip2_t & auditor_xip = block->get_cert()->get_auditor();
    std::string auditor_elect_addr;
    uint64_t auditor_elect_height = 0;
    get_elect_info(auditor_xip, auditor_elect_addr, auditor_elect_height);
    info.m_list.push_back(elect_item_t{auditor_elect_addr, auditor_elect_height});

    return info;
}

xentire_block_request_ptr_t xchain_downloader_t::create_request(uint64_t start_height, uint32_t count) {
    xentire_block_request_ptr_t ptr = std::make_shared<xentire_block_request_t>(
                                m_address,
                                start_height,
                                count);

    xsync_info("chain_downloader create_sync_request %s range[%lu,%lu]",
            m_address.c_str(), start_height, start_height+count-1);

    return ptr;
}

xsync_command_execute_result xchain_downloader_t::execute_download(uint64_t start_height, uint64_t end_height, enum_chain_sync_policy sync_policy, const vnetwork::xvnode_address_t &self_addr, const vnetwork::xvnode_address_t &target_addr, const std::string &reason) {
    std::string account_prefix;
    uint32_t table_id = 0;

    if (m_address == sys_drand_addr) {
    } else {
        if (!data::xdatautil::extract_parts(m_address, account_prefix, table_id)) {
            xsync_dbg("xchain_downloader_t::execute_download check fail : %s", m_address.c_str());
            return abort;
        }
        if (account_prefix == common::rec_table_base_address.to_string()) {
            // ignore
        } else if (account_prefix == common::zec_table_base_address.to_string()) {
            // if (!check_behind(end_height, sys_contract_rec_elect_zec_addr)) {
            if (!check_behind(end_height, rec_elect_zec_table_addr)) {
                xsync_info("chain_downloader on_behind(depend chain is syncing) %s,height=%lu,", m_address.c_str(), end_height);
                return abort;
            }
        } else if (account_prefix == common::con_table_base_address.to_string()) {
            // if (!check_behind(end_height, sys_contract_zec_elect_consensus_addr)) {
            if (!check_behind(end_height, zec_elect_shard_table_addr)) {
                xsync_info("chain_downloader on_behind(depend chain is syncing) %s,height=%lu,", m_address.c_str(), end_height);
                return abort;
            }
        } else if (account_prefix == common::eth_table_base_address.to_string()) {
        } else if (account_prefix == common::relay_table_base_address.to_string()) {
        } else {
            xsync_dbg("xchain_downloader_t::execute_download check fail: %s", m_address.c_str());
            return abort;
        }
    }

    int ret = m_sync_range_mgr.set_behind_info(start_height, end_height, sync_policy, self_addr, target_addr);    

    if (!ret) {
        return finish;
    }

    xsync_info("chain_downloader execute_download %s,local(height=%lu,) peer(height=%lu,) reason=%s %s -> %s, sync policy %d",
            m_address.c_str(),
            start_height,
            end_height,
            reason.c_str(),
            self_addr.to_string().c_str(),
            target_addr.to_string().c_str(),
            sync_policy);

    XMETRICS_GAUGE(metrics::xsync_downloader_block_behind, 1);

    uint64_t height = start_height;

    if (sync_policy == enum_chain_sync_policy_fast) {
        // xauto_ptr<xvblock_t> start_vblock = m_sync_store->get_latest_start_block(m_address, sync_policy);
        // data::xblock_ptr_t current_block = autoptr_to_blockptr(start_vblock);
        // if (!current_block->is_full_state_block() && current_block->get_height() == start_height) {
        //     if (!handle_fulltable(start_height, self_addr, target_addr)){
        //         return abort_overflow;
        //     }
        //     return wait_response;
        // }

        uint64_t latest_end_height = m_sync_store->get_latest_end_block_height(m_address, sync_policy);
        height = sync::derministic_height(latest_end_height, std::make_pair(start_height,end_height));
    }

    if (height > 0){
        return handle_next(height - 1);
    } else {
        return handle_next(height);
    }
}

xsync_command_execute_result xchain_downloader_t::execute_next_download(std::vector<data::xblock_ptr_t> &blocks, const vnetwork::xvnode_address_t &self_addr, const vnetwork::xvnode_address_t &from_addr) {
    uint32_t count = blocks.size();
    if (count == 0) {
        return ignore;
    }

    int64_t now = get_time();
    int64_t total_cost = now - m_request->send_time;

    xsync_info("chain_downloader on_response(overview) %s count(%u) cost(%ldms) %s",
        m_address.c_str(), count, total_cost, from_addr.to_string().c_str());

    m_ratelimit->feedback(total_cost, now);
    XMETRICS_GAUGE(metrics::xsync_downloader_response_cost, total_cost);

    if (false == sync_blocks_continue_check(blocks, "", false)) {
        xsync_warn("execute_next_download  blocks(address error) (%s)", blocks[0]->get_account().c_str());
        return ignore;
    }
    if (!data::is_table_address(common::xaccount_address_t {blocks[0]->get_account()}) 
        && !data::is_drand_address(common::xaccount_address_t { blocks[0]->get_account() })) {
        xsync_dbg("xsync_handler_t::blocks, address type error: %s", blocks[0]->get_account().c_str());
        return ignore;
    }

    // 1.verify shard-table block multi-sign
    if (!m_is_elect_chain){
        xblock_ptr_t &block = blocks[count-1];
        if (!check_auth(m_certauth, block)) {
            xsync_info("chain_downloader on_response(auth_failed) %s,height=%lu,", m_address.c_str(), block->get_height());
            return ignore;
        }
    }

    enum_chain_sync_policy sync_policy;
    if (!m_sync_range_mgr.get_sync_policy(sync_policy)) {
        xsync_info("chain_downloader on_response(not behind) %s,", m_address.c_str());
        return abort;
    }

    if (m_sync_range_mgr.get_current_sync_start_height() != blocks.begin()->get()->get_height()) {
        xsync_info("chain_downloader on_response expect height is %llu, but real height is %llu", m_sync_range_mgr.get_current_sync_start_height(), blocks.begin()->get()->get_height());
        return ignore;
    }

    auto next_block = blocks[blocks.size() - 1];
    init_committed_event_group();
    #if 0
    std::vector<top::base::xvblock_t *> processed_blocks;
    enum_result_code ret = pre_handle_block(blocks, m_is_elect_chain, next_block->get_height(), processed_blocks);
    if (ret != enum_result_code::success) {
        init_committed_event_group();
        return abort;
    }

    if (!m_sync_store->store_blocks(processed_blocks)) {
        xsync_warn("chain_downloader on_response(failed) fail to store blocks");
    }
    #endif

    // compare before and after
    for (uint32_t i = 0; i < count; i++) {
        xblock_ptr_t &block = blocks[i];
        enum_result_code ret = handle_block(block, next_block->get_height());

        if (ret == enum_result_code::success) {
            xsync_dbg("chain_downloader on_response(succ) %s,height=%lu,viewid=%lu,prev_hash:%s,",
                m_address.c_str(), block->get_height(), block->get_viewid(), to_hex_str(block->get_last_block_hash()).c_str());
        } else if (ret == enum_result_code::failed) {
            xsync_warn("chain_downloader on_response(failed) reason %d, block is: %s", ret, block->dump().c_str());
        }
    }


    xsync_info("chain_downloader on_response(total) %s,current(height=%lu,viewid=%lu,hash=%s) behind(height=%lu)",
        m_address.c_str(), next_block->get_height(), next_block->get_viewid(), to_hex_str(next_block->get_block_hash()).c_str(), m_sync_range_mgr.get_behind_height());

    if (notified_committed_event_group()) {
        return handle_next(next_block->get_height());
    }

    return wait_response;
}


xsync_command_execute_result xchain_downloader_t::execute_next_download(uint64_t height) {
    if (!notify_committed_event_group(height)) {
        return ignore;
    }

    if (notified_committed_event_group()) {
        return handle_next(height + 2);
    }

    return wait_response;
}

void xchain_downloader_t::init_committed_event_group(){
    m_wait_committed_event_group.clear();
}

void xchain_downloader_t::wait_committed_event_group(uint64_t height, uint64_t quota_height) {
    if (height <= (quota_height - xsync_store_t::m_undeterministic_heights)) {
        m_wait_committed_event_group.insert(height);
    }
}

bool xchain_downloader_t::notify_committed_event_group(uint64_t height) {
    if (m_wait_committed_event_group.empty()) {
        return false;
    }

    if (height != *(m_wait_committed_event_group.begin())) {
        return false;
    }

    m_wait_committed_event_group.erase(height);

    return true;
}

bool xchain_downloader_t::notified_committed_event_group() {
    if (m_wait_committed_event_group.empty()){
        return true;
    }

    return false;
}

bool xchain_object_t::pick(std::pair<uint64_t, uint64_t> &interval, vnetwork::xvnode_address_t &self_addr, vnetwork::xvnode_address_t &target_addr){
    if (m_end_height == 0){
        return false;
    }

    if (m_current_height > m_end_height) {
        return false;
    } else {
        interval.second = (m_current_height + BATCH_MAX_END) > m_end_height ? m_end_height : m_current_height + BATCH_MAX_END;
    }

    interval.first = m_current_height;

    self_addr = m_self_addr;
    target_addr = m_target_addr;
    m_picked_height = interval.second;
    return true;
}

uint64_t xchain_object_t::height() {
    return m_current_height;
}

void xchain_object_t::set_height(uint64_t height) {
    m_current_height = height;
}

void xchain_object_t::set_picked_height(uint64_t height) {
    m_picked_height = height;
}

uint64_t xchain_object_t::picked_height() {
    return m_picked_height;
}

void xchain_object_t::clear() {
    m_start_height = 0;
    m_end_height = 0;
}

bool xchain_object_t::check_and_fix_behind_height(int64_t now, xsync_store_face_t* xsync_store, const uint32_t sync_type, const std::string& address) {
    if (m_end_height == 0) {
        return false;
    }

    if ((m_regular_time == 0) || ((now - m_regular_time) > 60000)) {
        m_regular_time = now;
        m_connect_height = xsync_store->get_latest_end_block_height(address, (enum_chain_sync_policy)sync_type); // m_connect_height is commit + 2

        // check lost block
        if (m_connect_height < 2) { // 0, 1 
            m_current_height = m_connect_height + 1;
        } else if ((m_connect_height + 1) < m_current_height) {
            m_current_height = m_connect_height - 1;
            return true;
        }
    }

    if (m_current_height < m_connect_height) {
        xinfo("check_and_fix_behind_height fix height account is %s, current %llu reset  to new height %llu, sync_type %d", address.c_str(), m_current_height, m_connect_height, sync_type);
        m_current_height = m_connect_height;
    }

    return true;
}
NS_END2

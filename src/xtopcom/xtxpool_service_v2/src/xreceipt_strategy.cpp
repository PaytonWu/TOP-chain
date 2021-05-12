// Copyright (c) 2017-2020 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xtxpool_service_v2/xreceipt_strategy.h"

#include "xdata/xtableblock.h"
NS_BEG2(top, xtxpool_service_v2)

#define recover_unconfirmed_txs_interval (0xFF)  // every 256 seconds recover once.

#define receipt_resend_interval (0x3F)  // every 64 seconds resend once
#define shifting_for_receipt_resend_interval (6)
#define receipt_sender_select_num (2)

bool xreceipt_strategy_t::is_time_for_recover_unconfirmed_txs(uint64_t now) {
    return (now & recover_unconfirmed_txs_interval) == 0;
}

std::vector<data::xcons_transaction_ptr_t> xreceipt_strategy_t::make_receipts(data::xblock_t * block) {
    if (!block->is_lighttable() || !block->check_block_flag(base::enum_xvblock_flag_committed)) {
        xinfo("xreceipt_strategy_t::make_receipts block:%s", block->dump().c_str());
        return {};
    }

    data::xtable_block_t * tableblock = dynamic_cast<data::xtable_block_t *>(block);
    std::vector<data::xcons_transaction_ptr_t> sendtx_receipts;
    std::vector<data::xcons_transaction_ptr_t> recvtx_receipts;
    tableblock->create_txreceipts(sendtx_receipts, recvtx_receipts);
    sendtx_receipts.insert(sendtx_receipts.end(), recvtx_receipts.begin(), recvtx_receipts.end());
    return sendtx_receipts;
}

bool xreceipt_strategy_t::is_resend_node_for_talbe(uint64_t now, uint32_t table_id, uint16_t shard_size, uint16_t self_node_id) {
    // different table resend at different time by different advance node
    uint64_t random_num = now + (uint64_t)table_id;
    bool is_time_for_resend = ((random_num & receipt_resend_interval) == 0);
    uint16_t resend_node_pos = ((now >> shifting_for_receipt_resend_interval) + (uint64_t)table_id) % shard_size;
    xinfo("xreceipt_strategy_t::is_resend_node_for_talbe table:%d,now:%llu,interval0x%x,is_time_for_resend:%d,shard_size:%d,resend_node_pos:%d,self_node_id:%d",
          table_id,
          now,
          receipt_resend_interval,
          is_time_for_resend,
          shard_size,
          resend_node_pos,
          self_node_id);
    return (is_time_for_resend && resend_node_pos == self_node_id);
}

bool xreceipt_strategy_t::is_need_select_sender(base::enum_transaction_subtype subtype, uint32_t resend_time) {
    // resend recv tx need not select sender, because already selected sender by gmtime before.
    return (subtype == base::enum_transaction_subtype_confirm || resend_time == 0);
}

bool xreceipt_strategy_t::is_selected_sender(const data::xcons_transaction_ptr_t & cons_tx, uint32_t resend_time, uint16_t node_id, uint16_t shard_size) {
    // select 2 auditor to send the receipt
    uint32_t select_num = receipt_sender_select_num;
    // calculate a random position that means which node is selected to send the receipt
    // the random position change by resend_time for rotate the selected node, to avoid same node is selected continuously.
    uint32_t rand_pos = (base::xhash32_t::digest(cons_tx->get_transaction()->get_digest_str()) + resend_time) % shard_size;
    bool ret = is_selected_pos(node_id, rand_pos, select_num, shard_size);
    xinfo("xreceipt_strategy_t::is_selected_sender ret:%d tx:%s rand_pos:%u select_num:%u node_id:%u shard_size:%u resend_time:%u",
          ret,
          cons_tx->dump().c_str(),
          rand_pos,
          select_num,
          node_id,
          shard_size,
          resend_time);
    return ret;
}

uint32_t xreceipt_strategy_t::calc_resend_time(uint64_t tx_cert_time, uint64_t now) {
    return (now - tx_cert_time) >> shifting_for_receipt_resend_interval;
}

bool xreceipt_strategy_t::is_selected_pos(uint32_t pos, uint32_t rand_pos, uint32_t select_num, uint32_t size) {
    bool ret = false;
    xassert((select_num < size) && (pos < size));
    if (pos >= rand_pos) {
        return pos < rand_pos + select_num;
    } else {
        return (rand_pos + select_num > size) && (pos < (rand_pos + select_num) % size);
    }
}

NS_END2
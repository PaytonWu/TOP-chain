// Copyright (c) 2017-2020 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <string>

#include "xbase/xdata.h"
#include "xbasic/xmemory.hpp"
#include "xvledger/xvcertauth.h"
#include "xstore/xstore_face.h"
#include "xchain_timer/xchain_timer_face.h"
#include "xmbus/xmessage_bus.h"
#include "xtxpool_v2/xreceiptid_state_cache.h"

NS_BEG2(top, xtxpool_v2)

class xtxpool_resources_face {
public:
    virtual store::xstore_face_t * get_store() const = 0;
    virtual base::xvblockstore_t * get_vblockstore() const = 0;
    virtual base::xvcertauth_t * get_certauth() const =0;
    virtual mbus::xmessage_bus_face_t * get_bus() const = 0;
    virtual xreceiptid_state_cache_t & get_receiptid_state_cache() = 0;
    virtual void update_send_ids_after_add_rsp_id(const base::xreceiptid_state_ptr_t & receiptid_state, const std::set<base::xtable_shortid_t> & all_table_sids) = 0;
    virtual bool get_send_id_after_add_rsp_id(base::xtable_shortid_t self_sid, base::xtable_shortid_t peer_sid, uint64_t & send_id) const = 0;
};

NS_END2

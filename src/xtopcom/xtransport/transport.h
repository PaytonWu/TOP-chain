// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "transport_fwd.h"
#include "xbase/xdata.h"
#include "xbase/xlog.h"
#include "xbase/xobject.h"
#include "xbase/xpacket.h"
#include "xbase/xsocket.h"
#include "xbase/xthread.h"
#include "xbase/xtimer.h"
#include "xbase/xutl.h"
#include "xbasic/xbyte_buffer.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>

namespace top {
namespace transport {

class MultiThreadHandler;

class Transport {
public:
    virtual int Start(const std::string & local_ip, uint16_t local_port, MultiThreadHandler * message_handler) = 0;
    virtual void Stop() = 0;
    virtual int SendDataWithProp(std::string const & data, const std::string & peer_ip, uint16_t peer_port, UdpPropertyPtr & udp_property, uint16_t priority_flag = 0) = 0;

    virtual int32_t get_handle() = 0;
    virtual int get_socket_status() = 0;
    virtual std::string local_ip() = 0;
    virtual uint16_t local_port() = 0;

    virtual void register_on_receive_callback(on_receive_callback_t callback) = 0;
    virtual void unregister_on_receive_callback() = 0;

    virtual int SendPing(const xbyte_buffer_t & data, const std::string & peer_ip, uint16_t peer_port) = 0;
    virtual int SendPing(base::xpacket_t & packet) = 0;
    virtual int RegisterOfflineCallback(std::function<void(const std::string & ip, const uint16_t port)> cb) = 0;
    virtual int RegisterNodeCallback(std::function<int32_t(std::string const & node_addr, std::string const & node_sign)> cb) = 0;
    virtual int CheckRatelimitMap(const std::string & to_addr) = 0;

protected:
    Transport() {
    }
    virtual ~Transport() {
    }
};

typedef std::shared_ptr<Transport> TransportPtr;

}  // namespace transport

}  // namespace top

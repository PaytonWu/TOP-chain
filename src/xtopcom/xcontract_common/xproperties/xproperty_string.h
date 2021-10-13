// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xcontract_common/xproperties/xbasic_property.h"
#include "xcontract_common/xbasic_contract.h"
#include "xcontract_common/xcontract_state.h"

#include <string>

NS_BEG3(top, contract_common, properties)

class xtop_string_property: public xbasic_property_t {
public:
    xtop_string_property(xtop_string_property const&) = delete;
    xtop_string_property& operator=(xtop_string_property const&) = delete;
    xtop_string_property(xtop_string_property&&) = default;
    xtop_string_property& operator=(xtop_string_property&&) = default;
    ~xtop_string_property() = default;

    explicit xtop_string_property(std::string const & prop_name, contract_common::xbasic_contract_t * contract);

    void create() override final;
    void update(std::string const & prop_value);
    void clear();
    std::string query() const;
    // std::string query(common::xaccount_address_t const & contract) const;
};

using xstring_property_t = xtop_string_property;

NS_END3

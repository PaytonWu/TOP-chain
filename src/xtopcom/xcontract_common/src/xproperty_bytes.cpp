// Copyright (c) 2017-2021 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xcontract_common/xproperties/xproperty_bytes.h"

NS_BEG3(top, contract_common, properties)

xtop_bytes_property::xtop_bytes_property(std::string const & name, xcontract_face_t * contract)
  : xbasic_property_t{name, state_accessor::properties::xproperty_type_t::bytes, make_observer(contract)} {
}

xtop_bytes_property::xtop_bytes_property(std::string const & name, std::unique_ptr<xcontract_state_t> state_owned)
  : xbasic_property_t{name, state_accessor::properties::xproperty_type_t::bytes, std::move(state_owned)} {
}

void xtop_bytes_property::set(xbytes_t const & value) {
    assert(m_associated_contract != nullptr);
    assert(m_associated_contract->contract_state() != nullptr);
    m_associated_contract->contract_state()->set_property<state_accessor::properties::xproperty_type_t::bytes>(
        static_cast<state_accessor::properties::xtypeless_property_identifier_t>(m_id), value);
}

void xtop_bytes_property::clear() {
    assert(m_associated_contract != nullptr);
    assert(m_associated_contract->contract_state() != nullptr);
    m_associated_contract->contract_state()->clear_property(m_id);
}

xbytes_t xtop_bytes_property::value() const {
    if (m_associated_contract != nullptr) {
        assert(m_associated_contract->contract_state() != nullptr);
        return m_associated_contract->contract_state()->get_property<state_accessor::properties::xproperty_type_t::bytes>(
            static_cast<state_accessor::properties::xtypeless_property_identifier_t>(m_id));
    } else {
        assert(m_state_owned != nullptr);
        return m_state_owned->get_property<state_accessor::properties::xproperty_type_t::bytes>(static_cast<state_accessor::properties::xtypeless_property_identifier_t>(m_id));
    }
}

xbytes_t xtop_bytes_property::value(common::xaccount_address_t const & contract) const {
    assert(m_associated_contract != nullptr);
    assert(m_associated_contract->contract_state() != nullptr);
    return m_associated_contract->contract_state()->get_property<state_accessor::properties::xproperty_type_t::bytes>(
        static_cast<state_accessor::properties::xtypeless_property_identifier_t>(m_id), contract);
}

NS_END3

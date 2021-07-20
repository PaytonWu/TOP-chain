// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xchain_upgrade/xchain_data_processor.h"

#include "nlohmann/json.hpp"
#include "xchain_upgrade/xchain_data.h"
#include "xdata/xproperty.h"

using json = nlohmann::json;

namespace top
{
    namespace chain_reset
    {
        auto stake_property_json_parse = json::parse(stake_property_json);
        auto user_property_json_parse = json::parse(user_property_json);
        void xtop_chain_reset_center::get_reset_all_user_data(std::vector<reset_data_t> & data_vec) 
        {
            for (auto it = user_property_json_parse.begin(); it != user_property_json_parse.end(); it++) {
                common::xaccount_address_t account_address{it.key()};
                reset_data_t reset_data;
                reset_data.address = it.key();
                reset_data.top_balance = (it->count(data::XPROPERTY_BALANCE_AVAILABLE)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_AVAILABLE))) : 0;
                reset_data.burn_balance = (it->count(data::XPROPERTY_BALANCE_BURN)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_BURN))) : 0;
                reset_data.tgas_balance = (it->count(data::XPROPERTY_BALANCE_PLEDGE_TGAS)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_PLEDGE_TGAS))) : 0;
                reset_data.vote_balance = (it->count(data::XPROPERTY_BALANCE_PLEDGE_VOTE)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_PLEDGE_VOTE))) : 0;
                reset_data.lock_balance = (it->count(data::XPROPERTY_BALANCE_LOCK)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_LOCK))) : 0;
                reset_data.lock_tgas = (it->count(data::XPROPERTY_LOCK_TGAS)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_LOCK_TGAS))) : 0;
                reset_data.unvote_num = (it->count(data::XPROPERTY_UNVOTE_NUM)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_UNVOTE_NUM))) : 0;
                reset_data.create_time = (it->count(data::XPROPERTY_ACCOUNT_CREATE_TIME)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_ACCOUNT_CREATE_TIME))) : 0;
                reset_data.lock_token = (it->count(data::XPROPERTY_LOCK_TOKEN_KEY)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_LOCK_TOKEN_KEY))) : 0;
                if (it->count(data::XPROPERTY_PLEDGE_VOTE_KEY)) {
                    for(auto const & item : it->at(data::XPROPERTY_PLEDGE_VOTE_KEY)) {
                        std::string str = base::xstring_utl::base64_decode(item);
                        reset_data.pledge_vote.emplace_back(str);
                    }
                }
                reset_data.expire_vote = (it->count(data::XPROPERTY_EXPIRE_VOTE_TOKEN_KEY)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_EXPIRE_VOTE_TOKEN_KEY))) : 0;
                data_vec.emplace_back(reset_data);
            }
        }

        void xtop_chain_reset_center::get_reset_user_data(common::xaccount_address_t const &addr, chain_reset::reset_data_t & reset_data) {
            std::string account = addr.to_string();
            auto it = user_property_json_parse.find(account);
            if(it != user_property_json_parse.end())
            {
                reset_data.address = it.key();
                reset_data.top_balance = (it->count(data::XPROPERTY_BALANCE_AVAILABLE)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_AVAILABLE))) : 0;
                reset_data.burn_balance = (it->count(data::XPROPERTY_BALANCE_BURN)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_BURN))) : 0;
                reset_data.tgas_balance = (it->count(data::XPROPERTY_BALANCE_PLEDGE_TGAS)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_PLEDGE_TGAS))) : 0;
                reset_data.vote_balance = (it->count(data::XPROPERTY_BALANCE_PLEDGE_VOTE)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_PLEDGE_VOTE))) : 0;
                reset_data.lock_balance = (it->count(data::XPROPERTY_BALANCE_LOCK)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_LOCK))) : 0;
                reset_data.lock_tgas = (it->count(data::XPROPERTY_LOCK_TGAS)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_LOCK_TGAS))) : 0;
                reset_data.unvote_num = (it->count(data::XPROPERTY_UNVOTE_NUM)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_UNVOTE_NUM))) : 0;
                reset_data.create_time = (it->count(data::XPROPERTY_ACCOUNT_CREATE_TIME)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_ACCOUNT_CREATE_TIME))) : 0;
                reset_data.lock_token = (it->count(data::XPROPERTY_LOCK_TOKEN_KEY)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_LOCK_TOKEN_KEY))) : 0;
                if (it->count(data::XPROPERTY_PLEDGE_VOTE_KEY)) {
                    for(auto const & item : it->at(data::XPROPERTY_PLEDGE_VOTE_KEY)) {
                        std::string str = base::xstring_utl::base64_decode(item);
                        reset_data.pledge_vote.emplace_back(str);
                    }
                }
                reset_data.expire_vote = (it->count(data::XPROPERTY_EXPIRE_VOTE_TOKEN_KEY)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_EXPIRE_VOTE_TOKEN_KEY))) : 0;
            }
        }

        void xtop_chain_reset_center::get_reset_all_contract_data(std::vector<reset_data_t> & data_vec) 
        {
            for (auto it = stake_property_json_parse.begin(); it != stake_property_json_parse.end(); it++) {
                common::xaccount_address_t account_address{it.key()};
                reset_data_t reset_data;
                reset_data.address = it.key();
                reset_data.top_balance = (it->count(data::XPROPERTY_BALANCE_AVAILABLE)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_AVAILABLE))) : 0;
                reset_data.burn_balance = (it->count(data::XPROPERTY_BALANCE_BURN)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_BURN))) : 0;
                reset_data.tgas_balance = (it->count(data::XPROPERTY_BALANCE_PLEDGE_TGAS)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_PLEDGE_TGAS))) : 0;
                reset_data.vote_balance = (it->count(data::XPROPERTY_BALANCE_PLEDGE_VOTE)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_PLEDGE_VOTE))) : 0;
                reset_data.lock_balance = (it->count(data::XPROPERTY_BALANCE_LOCK)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_LOCK))) : 0;
                reset_data.lock_tgas = (it->count(data::XPROPERTY_LOCK_TGAS)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_LOCK_TGAS))) : 0;
                reset_data.unvote_num = (it->count(data::XPROPERTY_UNVOTE_NUM)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_UNVOTE_NUM))) : 0;
                reset_data.create_time = (it->count(data::XPROPERTY_ACCOUNT_CREATE_TIME)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_ACCOUNT_CREATE_TIME))) : 0;
                reset_data.lock_token = (it->count(data::XPROPERTY_LOCK_TOKEN_KEY)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_LOCK_TOKEN_KEY))) : 0;
                if (it->count(data::XPROPERTY_PLEDGE_VOTE_KEY)) {
                    for(auto const & item : it->at(data::XPROPERTY_PLEDGE_VOTE_KEY)) {
                        std::string str = base::xstring_utl::base64_decode(item);
                        reset_data.pledge_vote.emplace_back(str);
                    }
                }
                reset_data.expire_vote = (it->count(data::XPROPERTY_EXPIRE_VOTE_TOKEN_KEY)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_EXPIRE_VOTE_TOKEN_KEY))) : 0;
                data_vec.emplace_back(reset_data);
            }
        }

        void xtop_chain_reset_center::get_reset_contract_data(common::xaccount_address_t const &addr, reset_data_t & reset_data) {
            std::string account = addr.to_string();
            auto it = stake_property_json_parse.find(account);
            if(it != stake_property_json_parse.end())
            {
                reset_data.address = it.key();
                reset_data.top_balance = (it->count(data::XPROPERTY_BALANCE_AVAILABLE)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_AVAILABLE))) : 0;
                reset_data.burn_balance = (it->count(data::XPROPERTY_BALANCE_BURN)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_BURN))) : 0;
                reset_data.tgas_balance = (it->count(data::XPROPERTY_BALANCE_PLEDGE_TGAS)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_PLEDGE_TGAS))) : 0;
                reset_data.vote_balance = (it->count(data::XPROPERTY_BALANCE_PLEDGE_VOTE)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_PLEDGE_VOTE))) : 0;
                reset_data.lock_balance = (it->count(data::XPROPERTY_BALANCE_LOCK)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_BALANCE_LOCK))) : 0;
                reset_data.lock_tgas = (it->count(data::XPROPERTY_LOCK_TGAS)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_LOCK_TGAS))) : 0;
                reset_data.unvote_num = (it->count(data::XPROPERTY_UNVOTE_NUM)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_UNVOTE_NUM))) : 0;
                reset_data.create_time = (it->count(data::XPROPERTY_ACCOUNT_CREATE_TIME)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_ACCOUNT_CREATE_TIME))) : 0;
                reset_data.lock_token = (it->count(data::XPROPERTY_LOCK_TOKEN_KEY)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_LOCK_TOKEN_KEY))) : 0;
                if (it->count(data::XPROPERTY_PLEDGE_VOTE_KEY)) {
                    for(auto const & item : it->at(data::XPROPERTY_PLEDGE_VOTE_KEY)) {
                        std::string str = base::xstring_utl::base64_decode(item);
                        reset_data.pledge_vote.emplace_back(str);
                    }
                }
                reset_data.expire_vote = (it->count(data::XPROPERTY_EXPIRE_VOTE_TOKEN_KEY)) ? base::xstring_utl::touint64(static_cast<std::string>(it->at(data::XPROPERTY_EXPIRE_VOTE_TOKEN_KEY))) : 0;
            }
        }

        void xtop_chain_reset_center::get_reset_stake_string_property(common::xaccount_address_t const &addr, std::string const &property, std::string &value)
        {
            if (stake_property_json_parse.count(addr.to_string())) 
            {
                value = base::xstring_utl::base64_decode(stake_property_json_parse.at(addr.to_string()).at(property));
            }
        }

        void xtop_chain_reset_center::get_reset_stake_map_property(common::xaccount_address_t const &addr, std::string const &property, std::vector<std::pair<std::string, std::string>> &map)
        {
            if (stake_property_json_parse.count(addr.to_string())) 
            {
                auto data = stake_property_json_parse.at(addr.to_string()).at(property);
                for (auto _p = data.begin(); _p != data.end(); ++_p)
                {
                    map.push_back(std::make_pair(base::xstring_utl::base64_decode(_p.key()), base::xstring_utl::base64_decode(_p.value())));
                }
            }
        }

        void xtop_chain_reset_center::release_reset_property() {
            stake_property_json_parse.clear();
            user_property_json_parse.clear();
        }
    }
}
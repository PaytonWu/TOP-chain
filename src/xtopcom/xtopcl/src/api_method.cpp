#include "api_method.h"

#include "base/utility.h"
#include "task/request_task.h"
#include "task/task_dispatcher.h"
#include "xbase/xutl.h"
// TODO(jimmy) #include "xbase/xvledger.h"
#include "xcrypto.h"
#include "xcrypto/xckey.h"
#include "xcrypto_util.h"
#include "xdata/xnative_contract_address.h"
#include "xpbase/base/top_utils.h"

#include <dirent.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <cmath>

namespace xChainSDK {
using namespace xcrypto;
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ostringstream;
static const std::string SAFEBOX_ENDPOINT_ENV = "SAFEBOX_ENDPOINT";

bool check_pri_key(const std::string & str_pri) {
    return BASE64_PRI_KEY_LEN == str_pri.size() || HEX_PRI_KEY_LEN == str_pri.size();
}
bool check_account_address(const std::string& account)
{
    top::base::xvaccount_t _vaccount(account);
    return _vaccount.is_unit_address();
}

xJson::Value ApiMethod::get_response_from_daemon() {
    std::string daemon_host = "127.0.0.1:7000";
    auto safebox_endpoint = getenv(SAFEBOX_ENDPOINT_ENV.c_str());
    if (safebox_endpoint != NULL) {
        daemon_host = safebox_endpoint;
    }
    HttpClient client(daemon_host);
    xJson::Value j;
    j["method"] = "get";
    std::string token_request = j.toStyledString();
    string token_response_str;
    try {
        SimpleWeb::CaseInsensitiveMultimap header;
        header.insert({"Content-Type", "application/json"});
        auto token_response = client.request("POST", "/api/safebox", token_request, header);
        token_response_str = token_response->content.string();
    } catch (std::exception & e) {
        cout << e.what() << endl;
        return "";
    }
    xJson::Reader reader;
    xJson::Value token_response_json;
    if (!reader.parse(token_response_str, token_response_json)) {
        cout << "json parse error" << endl;
    } else if (token_response_json["status"].asString() != "ok") {
#ifdef DEBUG
        cout << "[debug]" << token_response_json["error"].asString() << endl;
#endif
    }

    return token_response_json;
}

string ApiMethod::get_account_from_daemon() {
    auto token_response_json = get_response_from_daemon();
    try {
        return token_response_json["account"].asString();
    } catch (...) {
        return "";
    }
}
int ApiMethod::get_eth_file(std::string& account) {
    top::base::xvaccount_t _vaccount(account);
    if (_vaccount.is_eth_address())
        std::transform(account.begin() + 1, account.end(), account.begin() + 1, ::tolower);
    std::vector<std::string> files = xChainSDK::xcrypto::scan_key_dir(g_keystore_dir);
    for (int i = 0; i < (int)files.size(); i++)
    {
        std::string file = files[i];
        std::transform(file.begin() + 1, file.end(), file.begin() + 1, ::tolower);
        if (file == account)
        {
            account = files[i];
            break;
        }
    }
    return 0;
}
string ApiMethod::get_prikey_from_daemon(std::ostringstream & out_str) {
    auto token_response_json = get_response_from_daemon();
    try {
        auto hex_ed_key = token_response_json["private_key"].asString();
        auto account = token_response_json["account"].asString();
        string base64_pri;
        if (!hex_ed_key.empty()) {
            top::base::xvaccount_t _vaccount(account);
            if (_vaccount.is_eth_address())
                get_eth_file(account);
            std::string path = g_keystore_dir + '/' + account;
            base64_pri = decrypt_keystore_by_key(hex_ed_key, path);
        }
        return base64_pri;
    } catch (...) {
        return "";
    }
}

int ApiMethod::set_prikey_to_daemon(const string & account, const string & pri_key, std::ostringstream & out_str, uint32_t expired_time) {
    std::string daemon_host = "127.0.0.1:7000";
    auto safebox_endpoint = getenv(SAFEBOX_ENDPOINT_ENV.c_str());
    if (safebox_endpoint != NULL) {
        daemon_host = safebox_endpoint;
    }
    std::string account_temp(account);
    top::base::xvaccount_t _vaccount(account);
    if (_vaccount.is_eth_address())
        std::transform(account_temp.begin() + 1, account_temp.end(), account_temp.begin() + 1, ::tolower);

    HttpClient client(daemon_host);
    xJson::Value j;
    j["method"] = "set";
    j["account"] = account_temp;
    j["private_key"] = pri_key;
    j["expired_time"] = expired_time;
    std::string token_request = j.toStyledString();
    string token_response_str;
    try {
        SimpleWeb::CaseInsensitiveMultimap header;
        header.insert({"Content-Type", "application/json"});
        auto token_response = client.request("POST", "/api/safebox", token_request, header);
        token_response_str = token_response->content.string();
    } catch (std::exception & e) {
        out_str << e.what() << endl;
        return 1;
    }
    xJson::Reader reader;
    xJson::Value token_response_json;
    if (!reader.parse(token_response_str, token_response_json)) {
        out_str << "connection error" << endl;
        return 1;
    } else if (token_response_json["status"].asString() == "fail") {
        out_str << token_response_json["error"].asString() << endl;
        return 1;
    }
    return 0;
}

bool ApiMethod::set_default_prikey(std::ostringstream & out_str, const bool is_query) {
    if (g_userinfo.account.size() == 0) {
        std::string str_pri = get_prikey_from_daemon(out_str);
        if (!str_pri.empty()) {
            set_g_userinfo(str_pri);
        } else {
            std::vector<std::string> files = xChainSDK::xcrypto::scan_key_dir(g_keystore_dir);
            std::vector<std::string> accounts;
            for (auto file : files) {
                if (check_account_address(file))
                    accounts.push_back(file);
            }
            if (accounts.size() < 1) {
                cout << "You do not have a TOP account, please create an account." << std::endl;
                return false;
            }
            if (accounts.size() > 1) {
                cout << "There are multiple accounts in your wallet, please set a default account first." << std::endl;
                return false;
            }
            if (is_query) {
                g_userinfo.account = accounts[0];
                return true;
            }
            // cout << "set_default_prikey:" <<accounts[0] <<endl;
            std::string str_pri = xChainSDK::xcrypto::import_existing_keystore(cache_pw, g_keystore_dir + "/" + accounts[0]);
            if (str_pri.empty()) {
                //cout << "Please set a default account by command `topio wallet setDefaultAccount`." << std::endl;
                return false;
            }
            if (xChainSDK::xcrypto::set_g_userinfo(str_pri)) {
#ifdef DEBUG
                cout << "[debug]" << g_userinfo.account << " has been set as the default account." << std::endl;
#endif
            } else {
               // cout << "Please set a default account by command `topio wallet setDefaultAccount`." << std::endl;
                return false;
            }
        }
    }
    return true;
}

void ApiMethod::tackle_null_query(std::ostringstream & out_str, std::string null_out) {
    auto tmp = out_str.str();
    xJson::Value jv;
    xJson::Reader reader;
    if (reader.parse(tmp, jv)) {
        auto data = jv["data"];
        if (data.isNull() || (data.isMember("value") && data["value"].isNull())) {
            out_str.str("");
            auto err_no = jv["errno"].asInt();
            auto errmsg = jv["errmsg"].asString();
            if (errmsg == "OK" && err_no == 0) {
                if (null_out.empty()) {
                    cout << "No data!" << endl;
                } else {
                    cout << null_out << endl;
                }
            } else {
                cout << "[" << err_no << "]Errmsg: " << errmsg << endl;
            }
        }
    }
}

void ApiMethod::tackle_send_tx_request(std::ostringstream & out_str) {
    auto result = out_str.str();
    xJson::Value jv;
    xJson::Reader reader;
    if (reader.parse(result, jv)) {
        auto tx_hash = jv["tx_hash"].asString();
        if (!tx_hash.empty()) {
            out_str.str("");
            cout << "Transaction hash: " << tx_hash << endl;
            cout << "Please use command 'topio querytx' to query transaction status later on!!!" << endl;
        }
    }
}

int ApiMethod::update_account(std::ostringstream & out_str) {
    xJson::Value root;
    return update_account(out_str, root);
}

int ApiMethod::update_account(std::ostringstream & out_str, xJson::Value & root) {
    if (!set_default_prikey(out_str)) {
        return 1;
    }

    get_token();

    std::ostringstream oss;
    if (get_account_info(oss, root) != 0) {
        return 1;
    }

    return 0;
}

int ApiMethod::set_pw_by_file(const string & pw_path, string & pw) {
    if (!pw_path.empty()) {
        std::ifstream pw_file(pw_path, std::ios::in);
        if (!pw_file) {
            cout << pw_path << " Open Error!" << std::endl;
            return 1;
        }

        std::getline(pw_file, pw);
    }
    return 0;
}

void ApiMethod::create_account(const int32_t & pf, const string & pw_path, std::ostringstream & out_str) {
    is_account = true;
    cache_pw = empty_pw;
    if (pf == 1) {
        if (!check_password()) {
            return;
        }
    }

    if (set_pw_by_file(pw_path, cache_pw) != 0) {
        return;
    }

    std::string dir;
    auto path = create_new_keystore(cache_pw, dir);
    out_str << "Successfully create an account locally!\n" << std::endl;

    out_str << "Account Address: " << g_userinfo.account << std::endl;
    out_str << "Owner public-Key: " << top::utl::xcrypto_util::get_base64_public_key(g_userinfo.private_key) << "\n\n";
    out_str << "You can share your public key and account address with anyone.Others need them to interact with you!" << std::endl;
    out_str << "You must nerver share the private key or account keystore file with anyone!They control access to your funds!" << std::endl;
    out_str << "You must backup your account keystore file!Without the file, you will not be able to access the funds in your account!" << std::endl;
    out_str << "You must remember your password!Without the password,it’s impossible to use the keystore file!" << std::endl;

    return;
}

void ApiMethod::create_key(std::string & owner_account, const int32_t & pf, const string & pw_path, std::ostringstream & out_str) {
    cache_pw = empty_pw;
    if (1 == pf) {
        if (!check_password()) {
            return;
        }
    }

    if (set_pw_by_file(pw_path, cache_pw) != 0) {
        return;
    }

    if (owner_account.size() == 0) {
        if (!set_default_prikey(out_str)) {
            out_str << "Create worker key failed, you should specify an account address, or set a default account." << endl;
            return;
        } else {
            owner_account = g_userinfo.account;
        }
    }

    std::string dir = "";
    auto path = create_new_keystore(cache_pw, dir, true, owner_account);
    out_str << "Successfully create an worker keystore file!\n" << std::endl;
    out_str << "Account Address: " << owner_account << std::endl;
    out_str << "Public Key: " << top::utl::xcrypto_util::get_base64_public_key(g_userinfo.private_key) << "\n\n";
    out_str << "You can share your public key with anyone.Others need it to interact with you!" << std::endl;
    out_str << "You must nerver share the private key or keystore file with anyone!They can use them to make the node malicious." << std::endl;
    out_str << "You must backup your keystore file!Without the file,you may not be able to send transactions." << std::endl;
    out_str << "You must remember your password!Without the password,it’s impossible to use the keystore file!" << std::endl;
    return;
}

std::unordered_map<std::string, std::string> ApiMethod::queryNodeInfos() {
    std::ostringstream oss;
    auto tmp = g_userinfo.account;
    g_userinfo.account = top::sys_contract_rec_registration_addr;
    std::string target = "";  // emtpy target means query all node infos
    api_method_imp_.queryNodeInfo(g_userinfo, target, oss);
    g_userinfo.account = tmp;

    std::unordered_map<std::string, std::string> node_infos;
    xJson::Reader reader;
    xJson::Value root;
    if (reader.parse(oss.str(), root)) {
        for (auto a : root["data"].getMemberNames()) {
            node_infos[a] = root["data"][a]["node_sign_key"].asString();
        }
    }
    return node_infos;
}

void ApiMethod::list_accounts(std::ostringstream & out_str) {
    auto account = get_account_from_daemon();
    if (account.empty()) {
#ifdef DEBUG
        out_str << "[debug]No default account" << std::endl;
#endif
    } else {
        g_userinfo.account = account;
    }
    std::vector<std::string> keys = scan_key_dir(g_keystore_dir);
    if (keys.size() == 0) {
        out_str << "There is no account in wallet." << std::endl;
        return;
    }
    std::reverse(keys.begin(), keys.end());

    std::vector<std::string> av;
    if (!g_userinfo.account.empty()) {
        av.push_back(g_userinfo.account);
    }

    struct account_info {
        std::string owner_key;
        std::vector<std::string> worker_keys;
    };
    std::unordered_map<std::string, account_info> aim;
    for (size_t i = 0; i < keys.size(); ++i) {
        auto path = g_keystore_dir + "/" + keys[i];
        auto key_info = attach_parse_keystore(path, out_str);
        std::string account = key_info["account_address"].asString();
        if (account.empty())
            account = key_info["account address"].asString();
        top::base::xvaccount_t _vaccount(account);
        if (_vaccount.is_eth_address())
            std::transform(account.begin() + 1, account.end(), account.begin() + 1, ::tolower);

        if (std::find(av.begin(), av.end(), account) == av.end())
        {
            av.push_back(account);
        }

        if (key_info["key_type"].asString() == "owner") {
            aim[account].owner_key = key_info["public_key"].asString();
        } else {
            aim[account].worker_keys.push_back(key_info["public_key"].asString());
        }
    }

    // map of account & corresponding sign public key
    auto node_infos = queryNodeInfos();
    size_t i = 0;
    for (auto account : av) {
        auto ai = aim[account];
        std::string status;
        if (account.size() > 0 && account == g_userinfo.account) {
            status = " [default account]";
        }

        out_str << "account #" << i++ << ": " << account << status << std::endl;

        if (ai.owner_key.size() != 0) {
            out_str << "owner public-key: " << ai.owner_key;
            if (ai.owner_key == node_infos[account]) {
                out_str << " [registered as minerkey]";
            }
            out_str << std::endl;
        }
        outAccountBalance(account, out_str);

        size_t j = 0;
        for (size_t j = 0; j < ai.worker_keys.size(); ++j) {
            if (0 == j) {
                out_str << "worker public-key list:" << std::endl;
            }
            out_str << "public key #" << j << ": " << ai.worker_keys[j];
            if (ai.worker_keys[j] == node_infos[account]) {
                out_str << " [registered as minerkey]";
            }
            out_str << std::endl;
        }
        if (i != av.size()) {
            out_str << "------------------------------------------------------\n\n";
        }
    }
    return;
}

void ApiMethod::set_default_account(const std::string & account, const string & pw_path, std::ostringstream & out_str) {
    std::string account_file(account);
    top::base::xvaccount_t _vaccount(account);
    if (_vaccount.is_eth_address())
        get_eth_file(account_file);
    const std::string store_path = g_keystore_dir + "/" + account_file;
    std::fstream store_file;
    store_file.open(store_path, std::ios::in);
    if (!store_file) {
        out_str << "The account's owner keystore file does not exist in wallet." << std::endl;
        return;
    }

    std::string ed_key;
    std::string pw = empty_pw;
    if (set_pw_by_file(pw_path, pw) != 0) {
        return;
    }
    ed_key = get_symmetric_ed_key(pw, store_path);
    std::string str_pri = import_existing_keystore(pw, store_path, true);
    if (str_pri.empty()) {
        std::cout << "Please Input Password." << std::endl;
        pw = input_hiding();
        ed_key = get_symmetric_ed_key(pw, store_path);
        str_pri = import_existing_keystore(pw, store_path);
    }

    if (!str_pri.empty() && set_prikey_to_daemon(account, ed_key, out_str) == 0) {
        out_str << "Set default account successfully." << std::endl;
    } else {
        out_str << "Set default account failed." << std::endl;
    }
    return;
}

void ApiMethod::reset_keystore_password(std::string & public_key, std::ostringstream & out_str) {
    bool found = false;
    std::string path = g_keystore_dir + "/";
    std::vector<std::string> keys = scan_key_dir(g_keystore_dir);
    for (size_t i = 0; i < keys.size(); ++i) {
        auto tmp_path = path + keys[i];
        auto key_info = parse_keystore(tmp_path);
        if (key_info["public_key"] == public_key) {
            found = true;
            path = tmp_path;
            break;
        }
    }
    if (!found) {
        cout << "No file with public_key " << public_key << endl;
        return;
    }

    std::cout << "Please Input Old Password. If the keystore has no password, press Enter directly." << std::endl;
    auto pw = input_hiding();
    if (0 == pw.size()) {
        pw = empty_pw;
    }

    auto new_pw = reset_keystore_pw(pw, path);
    if (!new_pw.empty()) {
        std::ostringstream oss;
        auto default_account = get_account_from_daemon();
        if (g_userinfo.account == default_account) {
            std::string ed_key = get_symmetric_ed_key(new_pw, path);
            if (set_prikey_to_daemon(default_account, ed_key, oss) == 0) {
                std::cout << "Reset default account successfully." << std::endl;
            } else {
                std::cout << "Reset default account failed." << std::endl;
            }
        }
    }
}

void ApiMethod::import_account(const int32_t & pf, std::ostringstream & out_str) {
    is_account = true;
    cache_pw = empty_pw;
    if (pf == 1) {
        if (!check_password()) {
            return;
        }
    }
    std::string pri_str;
    if (input_pri_key(pri_str) != 0)
        return;

    std::string dir;
    std::string path = create_new_keystore(cache_pw, dir, pri_str);
    if (path.empty())
        return;

    out_str << "Import successfully.\n" << std::endl;
    out_str << "Account Address: " << g_userinfo.account << std::endl;
    out_str << "Public-Key: " << top::utl::xcrypto_util::get_base64_public_key(g_userinfo.private_key) << "\n\n";
    return;
}

void ApiMethod::export_account(const std::string & account, std::ostringstream & out_str) {
    if (account.empty()) {
    /*    std::string account_temp = get_account_from_daemon();
        if (!account_temp.empty())
            g_userinfo.account = account_temp;
        else { */
            cache_pw = empty_pw;
            if (!set_default_prikey(out_str))
            {
                std::cout << "Please Input Password." << std::endl;
                cache_pw = input_hiding();
                if (!set_default_prikey(out_str))
                    return;
            }
     //   }
    } else {
        g_userinfo.account = account;
    }

    if (g_userinfo.account.empty())
    {
        out_str << "Please input account address." << std::endl;
        return;
    }
    std::vector<std::string> keys = scan_key_dir(g_keystore_dir);
    if (keys.size() == 0) {
        out_str << "There is no account in wallet." << std::endl;
        return;
    }
    for (size_t i = 0; i < keys.size(); ++i) {
        if (keys[i] != g_userinfo.account)
            continue;
        std::string pw = cache_pw;
        std::string keystore_file = g_keystore_dir + "/" + keys[i];
        std::string str_pri = import_existing_keystore(pw, keystore_file, true);
        if (str_pri.empty())
        {
            std::cout << "Please Input Password." << std::endl;
            pw = input_hiding();

            str_pri = import_existing_keystore(pw, keystore_file);
            if (str_pri.empty()) {
                out_str << "Password error！" << std::endl;
                return;
            }
        }
        out_str << "Export successfully.\n" << std::endl;
        out_str << "Keystore file: " << keystore_file << std::endl;
        out_str << "Account Address: " << g_userinfo.account << std::endl;
        out_str << "Private-Key: " << str_pri << "\n\n";

        std::ifstream keyfile(keystore_file, std::ios::in);
        if (!keyfile)
        {
             return;
        }
        out_str << keyfile.rdbuf() << std::endl;
        return;
    }
    out_str << "Account address error! The account does not exist." << std::endl;
    return;
}

int ApiMethod::set_default_miner(const std::string & pub_key, const std::string & pw_path, std::ostringstream & out_str) {
    if (g_keystore_dir.empty() || g_data_dir.empty()) {
        out_str << "invalid keystore path" << std::endl;
        return -1;
    }
    std::vector<std::string> files = xChainSDK::xcrypto::scan_key_dir(g_keystore_dir);
    std::string target_kf;
    std::string target_node_id;
    for (const auto & kf : files) {
        auto kf_path = g_keystore_dir + "/" + kf;

        xJson::Value key_info_js;
        std::ifstream keyfile(kf_path, std::ios::in);
        if (!keyfile) {
            out_str << "open keystore file:" << kf_path << " failed" << std::endl;
            continue;
        }
        std::stringstream buffer;
        buffer << keyfile.rdbuf();
        keyfile.close();
        std::string key_info = buffer.str();
        xJson::Reader reader;
        if (!reader.parse(key_info, key_info_js)) {
            out_str << "parse keystore file:" << kf_path << " failed" << std::endl;
            continue;
        }

        for (const auto & name : key_info_js.getMemberNames()) {
            if (name == "public_key") {
                if (pub_key == key_info_js[name].asString()) {
                    target_kf = kf_path;
                }
            }
            if (name == "account address" || name == "account_address") {
                target_node_id = key_info_js[name].asString();
                top::base::xvaccount_t _vaccount(target_node_id);
                if (_vaccount.is_eth_address())
                    std::transform(target_node_id.begin() + 1, target_node_id.end(), target_node_id.begin() + 1, ::tolower);
            }
        }  // end for (const auto & name...
        if (!target_kf.empty()) {
            break;
        }
    }  // end for (const auto &kf...

    if (target_kf.empty()) {
        out_str << "The key does not exist in wallet." << std::endl;
        return -1;
    }

    std::string base64_pri;
    std::string hex_pri_token;
    std::string pw = empty_pw;
    if (set_pw_by_file(pw_path, pw) != 0) {
        return -1;
    }
    // do not store private_key directly, using pri_token instend
    hex_pri_token = get_symmetric_ed_key(pw, target_kf);
    base64_pri = import_existing_keystore(pw, target_kf, true);
    if (base64_pri.empty()) {
        std::cout << "Please Input Password." << std::endl;
        pw = input_hiding();
        hex_pri_token = get_symmetric_ed_key(pw, target_kf);
        base64_pri = import_existing_keystore(pw, target_kf);
    }
    if (base64_pri.empty() || hex_pri_token.empty()) {
        out_str << "decrypt private token failed" << std::endl;
        out_str << "Set miner key failed." << std::endl;
        return -1;
    }

    std::string extra_config = g_data_dir + "/.extra_conf.json";
    xJson::Value key_info_js;
    std::ifstream keyfile(extra_config, std::ios::in);
    if (keyfile) {
        std::stringstream buffer;
        buffer << keyfile.rdbuf();
        keyfile.close();
        std::string key_info = buffer.str();
        xJson::Reader reader;
        // ignore any error when parse
        reader.parse(key_info, key_info_js);
    }

    key_info_js["default_miner_node_id"] = target_node_id;
    key_info_js["default_miner_public_key"] = pub_key;
    key_info_js["default_miner_keystore"] = target_kf;
    // key_info_js["default_miner_private_key"] = base64_pri_key;
    // key_info_js["default_miner_private_key"] = hex_pri_token;

    // expired_time = 0 meaning nerver expired
    // if (set_prikey_to_daemon(target_node_id, base64_pri_key, out_str, 0) != 0) {
    // if (set_prikey_to_daemon(target_node_id, hex_pri_token, out_str, 0) != 0) {
    if (set_prikey_to_daemon(pub_key, hex_pri_token, out_str, 0) != 0) {
        // if set worker key target_node_id, will confilict with topcl(account)
        out_str << "keep default miner nfo in cache failed" << std::endl;
        out_str << "Set miner key failed." << std::endl;
        return -1;
    }

    // dump new json to file
    xJson::StyledWriter new_sw;
    std::ofstream os;
    os.open(extra_config);
    if (!os.is_open()) {
        out_str << "dump file failed, Set miner key failed" << std::endl;
        out_str << "Set miner key failed." << std::endl;
        return -1;
    }
    os << new_sw.write(key_info_js);
    os.close();
    out_str << "Set miner key successfully." << std::endl;
    return 0;
}

void ApiMethod::create_chain_account(std::ostringstream & out_str) {
    cache_pw = " "; // the password of create account is fixed to be one whitespace

    if (!set_default_prikey(out_str)) {
        return;
    }
    get_token();

    api_method_imp_.create_account(g_userinfo);
}

void ApiMethod::import_key(std::string & base64_pri, std::ostringstream & out_str) {
    set_g_userinfo(base64_pri);
    string dir = "";
    auto path = create_new_keystore(empty_pw, dir, base64_pri, false);
    std::cout << "Public Key: " << top::utl::xcrypto_util::get_base64_public_key(g_userinfo.private_key) << std::endl;
    std::cout << "Keystore File Path: " << path << std::endl;
    auto ed_key = get_symmetric_ed_key(empty_pw, path);
    if (set_prikey_to_daemon(g_userinfo.account, ed_key, out_str) == 0) {
        out_str << g_userinfo.account << ": Import private key successfully." << std::endl;
    } else {
        out_str << "Import private key failed." << std::endl;
    }
}

void ApiMethod::transfer1(std::string & to, std::string & amount_d, std::string & note, std::string & tx_deposit_d, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    if (!check_account_address(to))
    {
        cout << "Invalid transfer account address." <<endl;
        return;
    }
    std::string from = g_userinfo.account;
    top::base::xvaccount_t _vaccount(to);
    if (_vaccount.is_eth_address())
        std::transform(to.begin() + 1, to.end(), to.begin() + 1, ::tolower);

    if (note.size() > 128) {
        std::cout << "note size: " << note.size() << " > maximum size 128" << endl;
        return;
    }

    uint64_t amount;  // = ASSET_TOP(amount_d);
    if (parse_top_double(amount_d, TOP_UNIT_LENGTH, amount) != 0)
        return;
    uint64_t tx_deposit;  // = ASSET_TOP(tx_deposit_d);
    if (parse_top_double(tx_deposit_d, TOP_UNIT_LENGTH, tx_deposit) != 0)
        return;

    if (tx_deposit != 0) {
        api_method_imp_.set_tx_deposit(tx_deposit);
    }
    api_method_imp_.transfer(g_userinfo, from, to, amount, note, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::estimategas(std::string & to, std::string & amount_d, std::string & note, std::string & tx_deposit_d, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    if (!check_account_address(to)) {
        cout << "Invalid transfer account address." << endl;
        return;
    }
    std::string from = g_userinfo.account;
    if (top::base::xvaccount_t::get_addrtype_from_account(to) == top::base::enum_vaccount_addr_type_secp256k1_eth_user_account)
        std::transform(to.begin() + 1, to.end(), to.begin() + 1, ::tolower);

    if (note.size() > 128) {
        std::cout << "note size: " << note.size() << " > maximum size 128" << endl;
        return;
    }

    uint64_t amount;  // = ASSET_TOP(amount_d);
    if (parse_top_double(amount_d, TOP_UNIT_LENGTH, amount) != 0)
        return;
    uint64_t tx_deposit;  // = ASSET_TOP(tx_deposit_d);
    if (parse_top_double(tx_deposit_d, TOP_UNIT_LENGTH, tx_deposit) != 0)
        return;

    if (tx_deposit != 0) {
        api_method_imp_.set_tx_deposit(tx_deposit);
    }
    api_method_imp_.estimategas(g_userinfo, from, to, amount, note, out_str);
    tackle_null_query(out_str);
}

void ApiMethod::query_tx(std::string & account, std::string & tx_hash, std::ostringstream & out_str) {
    if (account.empty()) {
        if (!set_default_prikey(out_str)) {
            out_str << "No default account found, you should specify an account address, or set a default account." << endl;
            return;
        }
    } else {
        g_userinfo.account = account;
    }
    api_method_imp_.getTransaction(g_userinfo, g_userinfo.account, tx_hash, out_str);
    tackle_null_query(out_str);
}

void ApiMethod::query_miner_info(std::string & account, std::ostringstream & out_str) {
    if (account.empty()) {
        if (!set_default_prikey(out_str)) {
            out_str << "No default account found, you should specify an account address, or set a default account." << endl;
            return;
        }
    } else {
        g_userinfo.account = account;
    }
    top::base::xvaccount_t _vaccount(g_userinfo.account);
    if (_vaccount.is_eth_address())
        std::transform(g_userinfo.account.begin() + 1, g_userinfo.account.end(), g_userinfo.account.begin() + 1, ::tolower);
    auto rtn = api_method_imp_.getStandbys(g_userinfo, g_userinfo.account);
    if (rtn) {
        api_method_imp_.getElectInfo(g_userinfo, g_userinfo.account);
    }
    api_method_imp_.queryNodeInfo(g_userinfo, g_userinfo.account, out_str);
    string null_out = g_userinfo.account + " account has not registered miner.";
    tackle_null_query(out_str, null_out);
}
int ApiMethod::parse_top_double(const std::string &amount, const uint32_t unit, uint64_t &out)
{
    if (!std::all_of(amount.begin(), amount.end(), [](char c) { return (c <= '9' && c >= '0') || c == '.'; }))
    {
        cout << "Data format is invalid." << endl;
        return 1;
    }
    int dot_count = count_if(amount.begin(), amount.end(), [](char c) { return c == '.'; });
    if (dot_count > 1)
    {
        cout << "Data format is invalid." << endl;
        return 1;
    }
    std::string input = amount;
    bool dot_found{false};
    size_t i{0};
    out = 0;
    for (; i < (size_t)std::min(TOP_MAX_LENGTH + 1, (int)input.size()); ++i)
    {
        if (input[i] == '.')
        {
            dot_found = true;
            break;
        }
        out *= 10;
        out += input[i] - '0';
    }
    if (i == input.size())
    {
        out *= (uint32_t)std::pow(10, unit);
        return 0; // parse finished
    }
    if (!dot_found)
    {
        cout << "Data format is invalid." << endl;
        return 1;
    }

    ++i;
    for (auto j = 0; j < (int)unit; ++j, ++i)
    {
        if (i >= input.size())
        {
            out = out * (uint32_t)std::pow(10, unit - j);
            return 0;
        }
        out *= 10;
        out += input[i] - '0';
    }

    return 0;
}
void ApiMethod::register_node(const std::string & mortgage_d,
                              const std::string & role,
                              const std::string & nickname,
                              const uint32_t & dividend_rate,
                              std::string & signing_key,
                              std::ostringstream & out_str) {
    std::ostringstream res;
    xJson::Value root;
    if (update_account(res, root) != 0) {
        return;
    }
    if (signing_key.empty()) {
        signing_key = top::utl::xcrypto_util::get_base64_public_key(g_userinfo.private_key);
    } else {
        std::vector<std::string> keys = scan_key_dir(g_keystore_dir);
        bool found = false;
        for (size_t i = 0; i < keys.size(); ++i) {
            auto path = g_keystore_dir + "/" + keys[i];
            auto key_info = parse_keystore(path);
            auto public_key = key_info["public_key"].asString();
            if (public_key == signing_key) {
                found = true;
                std::string account = key_info["account_address"].asString();
                if (account.empty())
                    account = key_info["account address"].asString();
                top::base::xvaccount_t _vaccount(account);
                if (_vaccount.is_eth_address())
                    std::transform(account.begin()+1, account.end(), account.begin()+1, ::tolower);
                if (account == g_userinfo.account) {
                    break;
                } else {
                    cout << "The miner_key does not match default account in wallet." << endl;
                    return;
                }
            }
        }
        if (!found) {
            cout << "No file with public_key " << signing_key << endl;
            return;
        }
    }

    auto node_infos = queryNodeInfos();
    if (node_infos.find(g_userinfo.account) != node_infos.end()) {
        cout << "Register miner failed." << endl;
        cout << g_userinfo.account << " is already a miner." << endl;
        return;
    }

    uint64_t mortgage;
    if (parse_top_double(mortgage_d, TOP_UNIT_LENGTH, mortgage) != 0)
        return;
    api_method_imp_.registerNode(g_userinfo, mortgage, role, nickname, signing_key, dividend_rate, out_str);
    auto result = out_str.str();
    xJson::Reader reader;
    string tx_hash;
    if (!reader.parse(result, root)) {
        cout << result << endl;
        return;
    } else {
        tx_hash = root["tx_hash"].asString();
    }
    cout << "Miner account address: " << g_userinfo.account << endl;
    cout << "Miner public_key: " << signing_key << endl;
    tackle_send_tx_request(out_str);

    const uint16_t query_interval = 30;
    size_t cnt = 1;
    while (1) {
        cout << "Miner registering..., " << cnt++ << endl;
        sleep(query_interval);
        std::ostringstream oss;
        api_method_imp_.getTransaction(g_userinfo, g_userinfo.account, tx_hash, oss);
        auto result = oss.str();
        xJson::Reader reader;
        string tx_hash;
        if (!reader.parse(result, root)) {
            cout << result << endl;
        } else {
            auto tx_state = root["data"]["tx_consensus_state"]["confirm_block_info"]["exec_status"].asString();
            if (tx_state == "success") {
                cout << "Successfully registering to the mining pool." << endl;
                return;
            } else if (tx_state == "failure") {
                cout << "Register miner failed." << endl;
                return;
            }
        }
    }
}

void ApiMethod::query_miner_reward(std::string & target, std::ostringstream & out_str) {
    if (target.empty()) {
        if (!set_default_prikey(out_str, true)) {
            return;
        }
        target = g_userinfo.account;
    } else {
        g_userinfo.account = target;
    }
    api_method_imp_.queryNodeReward(g_userinfo, target, out_str);
    tackle_null_query(out_str);
}

void ApiMethod::claim_miner_reward(std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    api_method_imp_.claimNodeReward(g_userinfo, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::set_dividend_ratio(const uint32_t & dividend_rate, const std::string & tx_deposit_d, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    uint64_t tx_deposit; // = ASSET_TOP(tx_deposit_d);
    if (parse_top_double(tx_deposit_d, TOP_UNIT_LENGTH, tx_deposit) != 0)
        return;
    if (tx_deposit != 0) {
        api_method_imp_.set_tx_deposit(tx_deposit);
    }
    api_method_imp_.setDividendRatio(g_userinfo, dividend_rate, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::set_miner_name(std::string & name, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    api_method_imp_.setNodeName(g_userinfo, name, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::change_miner_type(std::string & role, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    api_method_imp_.updateNodeType(g_userinfo, role, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::unregister_node(std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    auto node_infos = queryNodeInfos();
    if (node_infos.find(g_userinfo.account) == node_infos.end()) {
        cout << "Terminate failed." << endl;
        cout << g_userinfo.account << " is not a miner now." << endl;
        return;
    }
    api_method_imp_.unRegisterNode(g_userinfo, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::update_miner_info(const std::string & role,
                                  const std::string & name,
                                  const uint32_t & updated_deposit_type,
                                  const std::string & node_deposit_d,
                                  const uint32_t & dividend_rate,
                                  const std::string & node_sign_key,
                                  std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }

    std::vector<std::string> keys = scan_key_dir(g_keystore_dir);
    bool found = false;
    for (size_t i = 0; i < keys.size(); ++i) {
        auto path = g_keystore_dir + "/" + keys[i];
        auto key_info = parse_keystore(path);
        if (key_info["public_key"] == node_sign_key) {
            found = true;
            std::string account = key_info["account_address"].asString();
            if (account.empty())
                account = key_info["account address"].asString();

            if (account == g_userinfo.account) {
                break;
            } else {
                cout << "The miner key does not match default account in wallet." << endl;
                return;
            }
        }
    }
    if (!found) {
        cout << "No file with public_key " << node_sign_key << endl;
        return;
    }

    uint64_t node_deposit;  // = ASSET_TOP(node_deposit_d);
    if (parse_top_double(node_deposit_d, TOP_UNIT_LENGTH, node_deposit) != 0)
        return;
    api_method_imp_.updateNodeInfo(g_userinfo, role, name, updated_deposit_type, node_deposit, dividend_rate, node_sign_key, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::add_deposit(const std::string & deposit_d, std::ostringstream & out_str) {
    std::ostringstream res;
    xJson::Value root;
    if (update_account(res, root) != 0) {
        return;
    }

    uint64_t deposit;  // = ASSET_TOP(deposit_d);
    if (parse_top_double(deposit_d, TOP_UNIT_LENGTH, deposit) != 0)
        return;
    api_method_imp_.stake_node_deposit(g_userinfo, deposit, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::reduce_deposit(const std::string & deposit_d, std::ostringstream & out_str) {
    cout << "Do you confirm your remaining miner deposit after reducing is still enough for the miner." << endl;
    cout << "Comfirmed: Y or y" << endl;
    cout << "Unconfirmed：Any character other than Y/y" << endl;
    string si;
    cin >> si;
    if (si != "Y" && si != "y") {
        return;
    }
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    uint64_t deposit;  // = ASSET_TOP(deposit_d);
    if (parse_top_double(deposit_d, TOP_UNIT_LENGTH, deposit) != 0)
        return;
    api_method_imp_.unstake_node_deposit(g_userinfo, deposit, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::withdraw_deposit(std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }

    auto node_infos = queryNodeInfos();
    if (node_infos.find(g_userinfo.account) != node_infos.end()) {
        cout << "Withdraw failed." << endl;
        cout << g_userinfo.account << " miner has not terminated mining." << endl;
        return;
    }

    api_method_imp_.redeemNodeDeposit(g_userinfo, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::query_account(std::string & target, std::ostringstream & out_str) {
    if (target.size() == 0) {
        if (!set_default_prikey(out_str)) {
            return;
        }
    } else {
        g_userinfo.account = target;
        top::base::xvaccount_t _vaccount(target);
        if (_vaccount.is_eth_address())
            std::transform(g_userinfo.account.begin()+1, g_userinfo.account.end(), g_userinfo.account.begin()+1, ::tolower);
    }
    api_method_imp_.getAccount(g_userinfo, g_userinfo.account, out_str);
    tackle_null_query(out_str);
}

void ApiMethod::query_block(std::string & target, std::string & height, std::ostringstream & out_str) {
    if (target.size() == 0) {
        if (!set_default_prikey(out_str)) {
            return;
        }
    } else {
        g_userinfo.account = target;
    }
    if (height != "latest") {
        try {
            auto hi = std::stoull(height);
        } catch (...) {
            cout << "Could not convert: height=" << height << endl;
            cout << "Parameter 'height' should be an integer or 'latest'." << endl;
            return;
        }
    }
    api_method_imp_.getBlock(g_userinfo, g_userinfo.account, height, out_str);
    tackle_null_query(out_str);
}

void ApiMethod::getBlocksByHeight(std::string & target, std::string & height, std::ostringstream & out_str) {
    if (target.size() == 0) {
        if (!set_default_prikey(out_str)) {
            return;
        }
    } else {
        g_userinfo.account = target;
    }
    if (height != "latest") {
        try {
            auto hi = std::stoull(height);
        } catch (...) {
            cout << "Could not convert: height=" << height << endl;
            cout << "Parameter 'height' should be an integer or 'latest'." << endl;
            return;
        }
    }
    api_method_imp_.getBlocksByHeight(g_userinfo, g_userinfo.account, height, out_str);
    tackle_null_query(out_str);
}

void ApiMethod::chain_info(std::ostringstream & out_str) {
    api_method_imp_.make_private_key(g_userinfo.private_key, g_userinfo.account);
    api_method_imp_.getChainInfo(g_userinfo, out_str);
}

void ApiMethod::general_info(std::ostringstream & out_str) {
    api_method_imp_.make_private_key(g_userinfo.private_key, g_userinfo.account);
    api_method_imp_.getGeneralInfo(g_userinfo, out_str);
}

void ApiMethod::deploy_contract(const uint64_t & tgas_limit, const std::string & amount_d, const std::string & code_path, const std::string & tx_deposit_d, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    ifstream code_file(code_path);
    ostringstream tmp;
    tmp << code_file.rdbuf();
    string code = tmp.str();

    uint64_t amount;  // = ASSET_TOP(amount_d);
    if (parse_top_double(amount_d, TOP_UNIT_LENGTH, amount) != 0)
        return;
    uint64_t tx_deposit;  // = ASSET_TOP(tx_deposit_d);
    if (parse_top_double(tx_deposit_d, TOP_UNIT_LENGTH, tx_deposit) != 0)
        return;
    if (tx_deposit != 0) {
        api_method_imp_.set_tx_deposit(tx_deposit);
    }
    api_method_imp_.deployContract(g_userinfo, g_userinfo.contract.account, tgas_limit, amount, code, out_str);
    cout << "contract account: " << g_userinfo.contract.account << std::endl;
    tackle_send_tx_request(out_str);
}

void ApiMethod::call_contract(const std::string & amount_d,
                              const string & addr,
                              const std::string & func,
                              const string & params,
                              const std::string & tx_deposit_d,
                              std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    uint64_t amount;  // = ASSET_TOP(amount_d);
    if (parse_top_double(amount_d, TOP_UNIT_LENGTH, amount) != 0)
        return;
    uint64_t tx_deposit;  // = ASSET_TOP(tx_deposit_d);
    if (parse_top_double(tx_deposit_d, TOP_UNIT_LENGTH, tx_deposit) != 0)
        return;
    if (tx_deposit != 0) {
        api_method_imp_.set_tx_deposit(tx_deposit);
    }
    api_method_imp_.runContract(g_userinfo, amount, addr, func, params, out_str);
    tackle_send_tx_request(out_str);
}

/*
govern
*/
void ApiMethod::get_proposal(std::string & target, std::ostringstream & out_str) {
    api_method_imp_.make_private_key(g_userinfo.private_key, g_userinfo.account);
    api_method_imp_.queryProposal(g_userinfo, target, out_str);
    tackle_null_query(out_str);
}

void ApiMethod::cgp(std::ostringstream & out_str) {
    api_method_imp_.make_private_key(g_userinfo.private_key, g_userinfo.account);
    std::string target;
    api_method_imp_.getCGP(g_userinfo, target, out_str);
}

void ApiMethod::submit_proposal(uint8_t & type,
                                const std::string & target,
                                const std::string & value,
                                std::string & deposit_d,
                                uint64_t & effective_timer_height,
                                std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    uint64_t deposit; // = ASSET_TOP(deposit_d);
    if (parse_top_double(deposit_d, TOP_UNIT_LENGTH, deposit) != 0)
        return;
    api_method_imp_.submitProposal(g_userinfo, type, target, value, deposit, effective_timer_height, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::withdraw_proposal(const std::string & proposal_id, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    api_method_imp_.withdrawProposal(g_userinfo, proposal_id, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::tcc_vote(const std::string & proposal_id, const std::string & opinion, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    bool option = false;
    if (opinion == "true" || opinion == "TRUE") {
        option = true;
    }
    api_method_imp_.tccVote(g_userinfo, proposal_id, option, out_str);
    tackle_send_tx_request(out_str);
}

/*
resource
*/
void ApiMethod::stake_for_gas(std::string & amount_d, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    uint64_t amount; // = ASSET_TOP(amount_d);
    if (parse_top_double(amount_d, TOP_UNIT_LENGTH, amount) != 0)
        return;
    api_method_imp_.stakeGas(g_userinfo, g_userinfo.account, g_userinfo.account, amount, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::withdraw_fund(std::string & amount_d, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    uint64_t amount; // = ASSET_TOP(amount_d);
    if (parse_top_double(amount_d, TOP_UNIT_LENGTH, amount) != 0)
        return;
    api_method_imp_.unStakeGas(g_userinfo, g_userinfo.account, g_userinfo.account, amount, out_str);
    tackle_send_tx_request(out_str);
}

/*
staking
*/
void ApiMethod::stake_fund(uint64_t & amount, uint16_t & lock_duration, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    api_method_imp_.stakeVote(g_userinfo, amount, lock_duration, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::stake_withdraw_fund(uint64_t & amount, const std::string & tx_deposit_d, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    uint64_t tx_deposit;  // = ASSET_TOP(tx_deposit_d);
    if (parse_top_double(tx_deposit_d, TOP_UNIT_LENGTH, tx_deposit) != 0)
        return;
    if (tx_deposit != 0) {
        api_method_imp_.set_tx_deposit(tx_deposit);
    }
    api_method_imp_.unStakeVote(g_userinfo, amount, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::vote_miner(std::vector<std::pair<std::string, int64_t>> & vote_infos, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    std::map<std::string, int64_t> votes;
    for (auto p : vote_infos) {
        std::string account = p.first;
        top::base::xvaccount_t _vaccount(account);
        if (_vaccount.is_eth_address())
            std::transform(account.begin()+1, account.end(), account.begin()+1, ::tolower);
        votes[account] = p.second;
        cout << account << " " << p.second << endl;
    }
    api_method_imp_.voteNode(g_userinfo, votes, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::withdraw_votes(std::vector<std::pair<std::string, int64_t>> & vote_infos, std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    std::map<std::string, int64_t> votes;
    for (auto p : vote_infos) {
        votes[p.first] = p.second;
        cout << p.first << " " << p.second << endl;
    }
    api_method_imp_.unVoteNode(g_userinfo, votes, out_str);
    tackle_send_tx_request(out_str);
}

void ApiMethod::query_votes(std::string & account, std::ostringstream & out_str) {
    if (account.empty()) {
        if (!set_default_prikey(out_str)) {
            out_str << "No default account found, you should specify an account address, or set a default account." << endl;
            return;
        }
    } else {
        g_userinfo.account = account;
    }
    api_method_imp_.listVoteUsed(g_userinfo, g_userinfo.account, out_str);
    tackle_null_query(out_str);
}

void ApiMethod::query_reward(std::string & account, std::ostringstream & out_str) {
    if (account.empty()) {
        if (!set_default_prikey(out_str)) {
            out_str << "No default account found, you should specify an account address, or set a default account." << endl;
            return;
        }
    } else {
        g_userinfo.account = account;
    }
    api_method_imp_.queryVoterDividend(g_userinfo, g_userinfo.account, out_str);
    tackle_null_query(out_str);
}

void ApiMethod::claim_reward(std::ostringstream & out_str) {
    std::ostringstream res;
    if (update_account(res) != 0) {
        return;
    }
    api_method_imp_.claimVoterDividend(g_userinfo, out_str);
    tackle_send_tx_request(out_str);
}

ApiMethod::ApiMethod() {
}

ApiMethod::~ApiMethod() {
}

std::string ApiMethod::input_hiding() {
    std::system("stty -echo");
    std::string str;
    std::getline(std::cin, str, '\n');
    cin.clear();
    std::system("stty echo");
    return str;
}

std::string ApiMethod::input_no_hiding() {
    std::string str;
    std::getline(std::cin, str, '\n');
    cin.clear();
    return str;
}

bool ApiMethod::check_password() {
    if (is_reset_pw) {
        cout << "Please set a new password. The password must consist of Numbers and Letters, 8 to 16 characters. Pressing Ctrl+C can exit the command." << endl;
    } else if (is_account) {
        std::cout << "Please set a password for the account keystore file. The password must consist of Numbers and Letters, 8 to 16 characters." << std::endl;
    } else {
        std::cout << "Please set a password for the keystore file. The password must consist of Numbers and Letters, 8 to 16 characters." << std::endl;
    }
    g_pw_hint = "";  // set to default empty
    auto pw1 = input_hiding();
    if (pw1.size() < 8 || pw1.size() > 16) {
        std::cout << "Password error!" << std::endl;
        return check_password();
    }
    bool digit_included{false};
    bool letter_included{false};
    for (auto c : pw1) {
        if (c >= '0' && c <= '9')
            digit_included = true;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            letter_included = true;
    }
    if (!(digit_included && letter_included)) {
        std::cout << "Password error!" << std::endl;
        return check_password();
    }

    std::cout << "Please Input Password Again" << std::endl;
    auto pw2 = input_hiding();

    if (pw1 != pw2) {
        std::cout << "Passwords are not the same." << std::endl;
        return check_password();
    } else {
        cache_pw = pw1;
        std::cout << "Please set a password hint! If don't, there will be no hint when you forget your password." << std::endl;
        g_pw_hint = input_no_hiding();
        return true;
    }
}
int ApiMethod::input_pri_key(std::string& pri_str) {
    cout<<"Please input private key."<<endl;
    std::string pri_key = input_hiding();
    if (check_pri_key(pri_key) == false) {
        std::cout << "Private key length error!" << std::endl;
        return 1;
    }
    pri_str = pri_key;
    return 0;
}

void ApiMethod::outAccountBalance(const std::string & account, std::ostringstream & out_str) {
    std::ostringstream as;
    auto tmp = g_userinfo.account;
    g_userinfo.account = account;
    std::string q_account(account);
    top::base::xvaccount_t _vaccount(q_account);
    if (_vaccount.is_eth_address())
        std::transform(q_account.begin()+1, q_account.end(), q_account.begin()+1, ::tolower);
    api_method_imp_.getAccount(g_userinfo, q_account, as);
    g_userinfo.account = tmp;
    xJson::Reader reader;
    xJson::Value root;
    if (reader.parse(as.str(), root)) {
        if (root["data"]["balance"].isUInt64()) {
            auto balance = root["data"]["balance"].asUInt64();
            double top_balance = (double)balance / TOP_UNIT;
            out_str.setf(std::ios::fixed, std::ios::floatfield);
            out_str.precision(6);
            out_str << "balance: " << top_balance << " TOP" << std::endl;
            out_str << "nonce: " << root["data"]["nonce"].asUInt64() << std::endl;
        } else {
            out_str << "balance: -" << std::endl;
            out_str << "nonce: -" << std::endl;
        }
    }
}

void ApiMethod::dump_userinfo(const user_info & info) {
    // constexpr static auto key_len = 32;

    std::cout.width(16);
    std::cout.setf(std::ios::left);

    std::cout << "identity_token: " << info.identity_token << std::endl;
    std::cout << "account: " << info.account << std::endl;
    std::cout << "last_hash: " << info.last_hash << std::endl;
    std::cout << "balance: " << info.balance << std::endl;
    std::cout << "nonce: " << info.nonce << std::endl;
#ifdef DEBUG
// std::cout << "secret_key: " << info.secret_key << std::endl;
// std::cout << "sign_method: " << info.sign_method << std::endl;
// std::cout << "sign_version: " << info.sign_version << std::endl;
// std::string private_key = uint_to_str(info.private_key, uinfo::key_len);
// std::cout << "private_key: " << private_key.c_str() << std::endl;
// if (private_key.length() > 2) {
//     std::cout << "public_key: " << api_method_imp_.get_public_key(info.private_key) << std::endl;
// }

// std::cout << "child_account: " << info.child.account << std::endl;
// private_key = uint_to_str(info.child.private_key, uinfo::key_len);
// std::cout << "child_private_key: " << private_key.c_str() << std::endl;

// std::cout << "contract_account: " << info.contract.account << std::endl;
// private_key = uint_to_str(info.contract.private_key, uinfo::key_len);
// std::cout << "contract_private_key: " << private_key.c_str() << std::endl;
// auto table_id = top::data::xaccount_mapping::account_to_table_id(info.account);
// std::stringstream ss;
// ss << "(0x" << std::hex << std::setw(4) << std::setfill('0') << table_id << ")";
// std::cout << "table id: " << table_id << ss.str() << std::endl;
#endif
}

int ApiMethod::set_userinfo() {
    api_method_imp_.make_private_key(g_userinfo.private_key, g_userinfo.account);
    return 0;
}

void ApiMethod::set_keystore_path(const std::string & data_dir) {
    g_data_dir = data_dir;
    g_keystore_dir = data_dir + "/keystore";
}

void ApiMethod::change_trans_mode(bool use_http) {
    if (use_http) {
        g_server_host_port = config_file::instance()->get_string(net_setting, http_host);
        if (g_server_host_port.empty()) {
            g_server_host_port = SERVER_HOST_PORT_HTTP;
        }
        trans_base::s_defaule_mode = TransMode::HTTP;
        // std::cout << "Using trans mode - HTTP: " << g_server_host_port << std::endl;
    } else {
        g_server_host_port = config_file::instance()->get_string(net_setting, ws_host);
        if (g_server_host_port.empty()) {
            g_server_host_port = SERVER_HOST_PORT_WS;
        }
        trans_base::s_defaule_mode = TransMode::WS;
        // std::cout << "Using trans mode - WS: " << g_server_host_port  << std::endl;
    }
#if  defined(DEBUG) || defined(RELEASEDEBINFO)
    // std::cout << "[debug]edge_domain_name old: " << g_edge_domain << std::endl;
    char* topio_home = getenv("TOPIO_HOME");
//    std::cout <<"data_dir:" << topio_home << std::endl;
    if (topio_home) {
        g_data_dir = topio_home;
//        std::cout <<"data_dir:" << g_data_dir << std::endl;
    }
    auto edge_config_path = g_data_dir + "/.edge_config.json";
    std::ifstream edge_config_file(edge_config_path, std::ios::in);
    if (!edge_config_file) {
        std::cout << "Edge domain name file: " << edge_config_path << " Not Exist" << std::endl;
    } else {
        std::stringstream buffer;
        buffer << edge_config_file.rdbuf();
        string edge_info = buffer.str();
        xJson::Reader reader;
        xJson::Value edge_info_js;
        if (!reader.parse(edge_info, edge_info_js)) {
            std::cout << "Edge domain name file: " << edge_config_path << " parse error" << endl;
        } else {
            if (!edge_info_js["edge_ip"].asString().empty()) {
                g_server_host_port = edge_info_js["edge_ip"].asString();
            }
            if (!edge_info_js["edge_domain_name"].asString().empty()) {
                g_edge_domain = edge_info_js["edge_domain_name"].asString();
            }
        }
    }

    std::cout << "[debug]edge_domain_name: " << g_edge_domain << std::endl;
    std::cout << "[debug]edge_ip: " << g_server_host_port << std::endl;
#endif
}

void ApiMethod::get_token() {
    api_method_imp_.passport(g_userinfo);
    sleep(1);
}

int ApiMethod::get_account_info(std::ostringstream & out_str, xJson::Value & root) {
    api_method_imp_.getAccount(g_userinfo, g_userinfo.account, out_str);
    auto result = out_str.str();
    xJson::Reader reader;
    if (reader.parse(result, root)) {
        if (root["data"].empty()) {
            cout << g_userinfo.account << " not found on chain!" << endl;
            return 1;
        }
    } else {
        cout << result << endl;
        return 1;
    }
    sleep(1);
    return 0;
}

void ApiMethod::block_prune(std::string & prune_enable, std::ostringstream & out_str) {
    top::base::xstring_utl::tolower_string(prune_enable);
    if (prune_enable != "off" && prune_enable != "on") {
        out_str << "Please set auto prune data to On or Off." << std::endl;
        return;
    }
    std::string extra_config = g_data_dir + "/.extra_conf.json";
    xJson::Value key_info_js;
    std::ifstream keyfile(extra_config, std::ios::in);
    if (keyfile) {
        std::stringstream buffer;
        buffer << keyfile.rdbuf();
        keyfile.close();
        std::string key_info = buffer.str();
        xJson::Reader reader;
        // ignore any error when parse
        reader.parse(key_info, key_info_js);
    }

    key_info_js["auto_prune_data"] = prune_enable;

    // dump new json to file
    xJson::StyledWriter new_sw;
    std::ofstream os;
    os.open(extra_config);
    if (!os.is_open()) {
        out_str << "Dump file failed" << std::endl;
        return;
    }
    os << new_sw.write(key_info_js);
    os.close();
    if (prune_enable == "off")
        out_str << "Set auto prune data Off successfully." << std::endl;
    else if (prune_enable == "on")
        out_str << "Set auto prune data On successfully." << std::endl;
}
}  // namespace xChainSDK

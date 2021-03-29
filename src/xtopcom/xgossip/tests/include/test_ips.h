// Copyright (c) 2017-2019 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <vector>

std::map<std::string, std::vector<std::string>> global_test_ips;

inline void InitGlobalTestIps() {
    global_test_ips["0"] = std::vector<std::string>{
        "192.168.50.81",
        "192.168.50.82",
        "192.168.50.84"
    };
    global_test_ips["1-0"] = std::vector<std::string>{
        "192.168.50.85",
        "192.168.50.86",
        "192.168.50.91",
    };
    global_test_ips["1-1"] = std::vector<std::string>{
        "192.168.50.92",
        "192.168.50.94",
        "192.168.50.95",
    };
    global_test_ips["1-2"] = std::vector<std::string>{
        "192.168.50.96",
        "192.168.50.97",
        "192.168.50.98",
    };
    global_test_ips["2-0"] = std::vector<std::string>{
        "192.168.50.99",
        "192.168.50.100",
        "192.168.50.101",
    };
    global_test_ips["2-1"] = std::vector<std::string>{
        "192.168.50.102",
        "192.168.50.104",
        "192.168.50.105",
    };
    global_test_ips["2-2"] = std::vector<std::string>{
        "192.168.50.106",
        "192.168.50.111",
        "192.168.50.112",
    };
    global_test_ips["2-3"] = std::vector<std::string>{
        "192.168.50.113",
        "192.168.50.114",
        "192.168.50.115",
    };
    global_test_ips["2-4"] = std::vector<std::string>{
        "192.168.50.116",
        "192.168.50.117",
        "192.168.50.118",
    };
    global_test_ips["2-5"] = std::vector<std::string>{
        "192.168.50.119",
        "192.168.50.120",
        "192.168.50.164",
    };
    global_test_ips["2-6"] = std::vector<std::string>{
        "192.168.50.165",
        "192.168.50.166",
        "192.168.50.147",
    };
    global_test_ips["2-7"] = std::vector<std::string>{
        "192.168.50.161",
        "192.168.50.123",
        "192.168.50.124",
    };
    global_test_ips["2-8"] = std::vector<std::string>{
        "192.168.50.125",
        "192.168.50.126",
        "192.168.50.131",
    };
    global_test_ips["3-0"] = std::vector<std::string>{
        "192.168.50.132",
        "192.168.50.133",
        "192.168.50.134",
    };
    global_test_ips["3-1"] = std::vector<std::string>{
        "192.168.50.135",
        "192.168.50.136",
        "192.168.50.137",
    };
    global_test_ips["3-2"] = std::vector<std::string>{
        "192.168.50.138",
        "192.168.50.139",
        "192.168.50.141",
    };
    global_test_ips["3-3"] = std::vector<std::string>{
        "192.168.50.142",
        "192.168.50.143",
        "192.168.50.144",
    };
    global_test_ips["3-4"] = std::vector<std::string>{
        "192.168.50.162",
        "192.168.50.163",
        "192.168.50.140"
    };
}
static std::vector<std::string> global_test_ip_vec{
    // 0
    "192.168.50.81",
    "192.168.50.82",
    "192.168.50.84",
    // 1
    "192.168.50.85",
    "192.168.50.86",
    "192.168.50.91",

    "192.168.50.92",
    "192.168.50.94",
    "192.168.50.95",

    "192.168.50.96",
    "192.168.50.97",
    "192.168.50.98",
    // 2 -0
    "192.168.50.99",
    "192.168.50.100",
    "192.168.50.101",
    // 2 -1

    "192.168.50.102",
    "192.168.50.104",
    "192.168.50.105",
    // 2 -2

    "192.168.50.106",
    "192.168.50.111",
    "192.168.50.112",
    // 2 -3

    "192.168.50.113",
    "192.168.50.114",
    "192.168.50.115",
    // 2 -4

    "192.168.50.116",
    "192.168.50.117",
    "192.168.50.118",
    // 2 -5

    "192.168.50.119",
    "192.168.50.120",
    "192.168.50.164",
    // 2 -6

    "192.168.50.165",
    "192.168.50.166",
    "192.168.50.147",
    // 2 -7

    "192.168.50.161",
    "192.168.50.123",
    "192.168.50.124",
    // 2 -8

    "192.168.50.125",
    "192.168.50.126",
    "192.168.50.131",
    // 3 - 0
    "192.168.50.132",
    "192.168.50.133",
    "192.168.50.134",
    // 3 - 1

    "192.168.50.135",
    "192.168.50.136",
    "192.168.50.137",
    // 3 - 2

    "192.168.50.138",
    "192.168.50.139",
    "192.168.50.141",
    // 3 - 3

    "192.168.50.142",
    "192.168.50.143",
    "192.168.50.144",
    // 3 - 4

    "192.168.50.162",
    "192.168.50.163",
    "192.168.50.140"
};
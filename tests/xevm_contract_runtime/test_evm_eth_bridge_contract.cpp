#include "tests/xevm_contract_runtime/test_vem_eth_bridge_contract_fixture.h"
#include "xbasic/xhex.h"
#include "xevm_common/rlp.h"
#include "xevm_common/xabi_decoder.h"
#include "xevm_common/xeth/xethash.h"
#include "xdepends/include/ethash/ethash.hpp"

#define private public
#include "xevm_common/xeth/xeth_header.h"
#include "xevm_contract_runtime/xevm_sys_contract_face.h"

namespace top {
namespace tests {

using namespace contract_runtime::evm::sys_contract;
using namespace evm_common;
using namespace evm_common::eth;
using namespace ethash;

void header_rlp_bytes_decode(const char * hex_input) {
    std::error_code ec;
    std::vector<xeth_header_t> headers;
    auto bytes = top::from_hex(hex_input, ec);
    auto item = RLP::decode_once(bytes);
    auto header_bytes = item.decoded[0];
    xeth_header_t header;
    header.decode_rlp(header_bytes);
    header.print();
}

TEST(stream, bytes_encode_decode) {
    base::xstream_t stream(base::xcontext_t::instance());
    xbytes_t bytes1{1, 3, 5, 7, 9};
    xbytes_t bytes2{2, 4, 6, 8, 0};
    stream << bytes1;
    stream << bytes2;
    auto stream_str = std::string(reinterpret_cast<char *>(stream.data()), static_cast<uint32_t>(stream.size()));

    base::xstream_t stream_decode(base::xcontext_t::instance(), (uint8_t *)(stream_str.c_str()), static_cast<uint32_t>(stream_str.size()));
    xbytes_t bytes1_decode;
    xbytes_t bytes2_decode;
    stream_decode >> bytes1_decode;
    stream_decode >> bytes2_decode;

    EXPECT_EQ(bytes1, bytes1_decode);
    EXPECT_EQ(bytes2, bytes2_decode);
}

TEST_F(xcontract_fixture_t, header_encode_decode) {
    evm_common::eth::xeth_header_t header;
    evm_common::eth::xeth_header_t header_decode;
    header.parent_hash = static_cast<evm_common::h256>(UINT32_MAX - 1);
    header.uncle_hash = static_cast<evm_common::h256>(UINT32_MAX - 2);
    header.miner = static_cast<evm_common::eth::Address>(UINT32_MAX - 3);
    header.state_merkleroot = static_cast<evm_common::h256>(UINT32_MAX - 4);
    header.tx_merkleroot = static_cast<evm_common::h256>(UINT32_MAX - 5);
    header.receipt_merkleroot = static_cast<evm_common::h256>(UINT32_MAX - 6);
    header.bloom = static_cast<evm_common::eth::LogBloom>(UINT32_MAX - 7);
    header.mix_digest = static_cast<evm_common::h256>(UINT32_MAX - 8);
    header.nonce = static_cast<evm_common::h64>(UINT32_MAX - 9);
    header.difficulty = static_cast<evm_common::bigint>(UINT64_MAX - 1);
    header.number = UINT64_MAX - 2;
    header.gas_limit = UINT64_MAX - 3;
    header.gas_used = UINT64_MAX - 4;
    header.time = UINT64_MAX - 5;
    header.base_fee = static_cast<evm_common::bigint>(UINT64_MAX - 6);
    header.extra = {1, 3, 5, 7};

    EXPECT_TRUE(contract.set_header(header.hash(), header));
    EXPECT_TRUE(contract.get_header(header.hash(), header_decode));

    EXPECT_EQ(header.parent_hash, header_decode.parent_hash);
    EXPECT_EQ(header.uncle_hash, header_decode.uncle_hash);
    EXPECT_EQ(header.miner, header_decode.miner);
    EXPECT_EQ(header.state_merkleroot, header_decode.state_merkleroot);
    EXPECT_EQ(header.tx_merkleroot, header_decode.tx_merkleroot);
    EXPECT_EQ(header.receipt_merkleroot, header_decode.receipt_merkleroot);
    EXPECT_EQ(header.bloom, header_decode.bloom);
    EXPECT_EQ(header.difficulty, header_decode.difficulty);
    EXPECT_EQ(header.number, header_decode.number);
    EXPECT_EQ(header.gas_limit, header_decode.gas_limit);
    EXPECT_EQ(header.gas_used, header_decode.gas_used);
    EXPECT_EQ(header.time, header_decode.time);
    EXPECT_EQ(header.extra, header_decode.extra);
    EXPECT_EQ(header.mix_digest, header_decode.mix_digest);
    EXPECT_EQ(header.nonce, header_decode.nonce);
    EXPECT_EQ(header.base_fee, header_decode.base_fee);
}

TEST_F(xcontract_fixture_t, test_last_hash_property) {
    EXPECT_EQ(contract.get_last_hash(), h256());
    EXPECT_TRUE(contract.set_last_hash(h256(9999)));
    EXPECT_EQ(contract.get_last_hash(), h256(9999));
}

TEST_F(xcontract_fixture_t, test_headers_property) {
    for (auto i = 0; i < 10; i++) {
        xeth_header_t header;
        EXPECT_FALSE(contract.get_header(h256(i), header));
    }
    for (auto i = 0; i < 100; i++) {
        xeth_header_t header;
        header.number = i;
        EXPECT_TRUE(contract.set_header(h256(i), header));
    }
    for (auto i = 0; i < 100; i++) {
        xeth_header_t header;
        header.number = i;
        xeth_header_t get_header;
        EXPECT_TRUE(contract.get_header(h256(i), get_header));
        EXPECT_EQ(header.hash(), get_header.hash());
        return;
    }
    for (auto i = 0; i < 50; i++) {
        EXPECT_TRUE(contract.remove_header(h256(i)));
    }
    for (auto i = 0; i < 100; i++) {
        if (i < 50) {
            xeth_header_t get_header;
            EXPECT_FALSE(contract.get_header(h256(i), get_header));
        } else {
            xeth_header_t header;
            header.number = i;
            xeth_header_t get_header;
            EXPECT_TRUE(contract.get_header(h256(i), get_header));
            EXPECT_EQ(header.hash(), get_header.hash());
        }
    }
}

TEST_F(xcontract_fixture_t, test_headers_summary_property) {
    for (auto i = 0; i < 10; i++) {
        xeth_header_info_t info;
        EXPECT_FALSE(contract.get_header_info(h256(i), info));
    }
    for (auto i = 0; i < 100; i++) {
        xeth_header_info_t info;
        info.number = i;
        EXPECT_TRUE(contract.set_header_info(h256(i), info));
    }
    for (auto i = 0; i < 100; i++) {
        xeth_header_info_t info;
        info.number = i;
        xeth_header_info_t get_info;
        EXPECT_TRUE(contract.get_header_info(h256(i), get_info));
        EXPECT_EQ(info.to_string(), get_info.to_string());
    }
    for (auto i = 0; i < 50; i++) {
        EXPECT_TRUE(contract.remove_header_info(h256(i)));
    }
    for (auto i = 0; i < 100; i++) {
        if (i < 50) {
            xeth_header_info_t info;
            EXPECT_FALSE(contract.get_header_info(h256(i), info));
        } else {
            xeth_header_info_t info;
            info.number = i;
            xeth_header_info_t get_info;
            EXPECT_TRUE(contract.get_header_info(h256(i), get_info));
            EXPECT_EQ(info.to_string(), get_info.to_string());
        }
    }
}

TEST_F(xcontract_fixture_t, test_effective_hash_property) {
    for (auto i = 0; i < 10; i++) {
        h256 info;
        EXPECT_EQ(contract.get_effective_hash(i), h256());
    }
    for (auto i = 0; i < 100; i++) {
        EXPECT_TRUE(contract.set_effective_hash(i, h256(i)));
    }
    for (auto i = 0; i < 100; i++) {
        EXPECT_EQ(contract.get_effective_hash(i), h256(i));
    }
    for (auto i = 0; i < 50; i++) {
        EXPECT_TRUE(contract.remove_effective_hash(i));
    }
    for (auto i = 0; i < 100; i++) {
        if (i < 50) {
            EXPECT_EQ(contract.get_effective_hash(i), h256());
        } else {
            EXPECT_EQ(contract.get_effective_hash(i), h256(i));
        }
    }
}

TEST_F(xcontract_fixture_t, test_all_hashes_property) {
    std::set<h256> h10{h256(1), h256(2), h256(3), h256(4), h256(5)};
    EXPECT_TRUE(contract.set_hashes(10, h10));
    auto hashes = contract.get_hashes(10);
    EXPECT_EQ(h10, hashes);
    EXPECT_TRUE(contract.remove_hashes(10));
    hashes = contract.get_hashes(10);
    EXPECT_TRUE(hashes.empty());
}

#include "test_evm_eth_bridge_contract_data.cpp"
// parent_hash: 688cbef142d64af10ff3bfed7463d11cc096493b926c8257e6dc3ab8ea8ee830
// uncle_hash: 1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347
// miner: 5a0b54d5dc17e0aadc383d2db43b0a0d3e029c4c
// state_merkleroot: 0dde65ccd7eb91b96ddd017e5b6983b0a86ca49f9b0ad8edb3f6bbcf4818af5c
// tx_merkleroot: 5e5b28e01547c6b46eab83f82062a513e42dec27a1a2ad3dc51dea6889f6dce0
// receipt_merkleroot: a8a266ccda44bc2fa160c0190b447932a2f2b5b34a2a178175baa38331b713e7
// bloom: 54f7de3e69a6dfc15bc046f2a2c8423b8286907297b5400c083566f9beca8107be8cf1393feedefcf7587f9350232df55aa7a2519d2d6d37ab96c0d420e02a702a40486da339e7abaaa6222fbb32f0e40b460c44b4723e160b465d6ac5c513fe5f02200b423e0b6c254551cd28956ff9783abc5bb87fcccacae3e5f8b85bc0369cb087b88e4f1b71937d15fa2997673fc2059d6731f22568a7e32cd4e3fd2da0e2fb96a333bca54b567b639ea8039e8e9ea0820b7ae6d04d4f2b77e21aadd117c03fb6a27f201feb3b652293b09b31504b3e026c1edebcd573c263b32b75f882d5b8284eae1445252217341091f022c6a6e3a7d12d5889508e08adf86acfd26d
// difficulty: 7652263722236960
// number: 12969999
// gas_limit: 29970301
// gas_used: 18001521
// time: 1628233653
// extra: d883010a06846765746888676f312e31362e36856c696e7578
// mix_digest: 51b6613a3f989905ea1c55a095ad8b1e1d7caabb9f890a60c880710db40eb294
// nonce: 99dcdb535b6987c1
// hash: 749c4c2c82582cba3489bce142d6c776d50a5c18b3aeaf3cdc68b27650d0a6b6
// base_fee: 27986467855

// parent_hash: 749c4c2c82582cba3489bce142d6c776d50a5c18b3aeaf3cdc68b27650d0a6b6
// uncle_hash: 1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347
// miner: 00192fb10df37c9fb26829eb2cc623cd1bf599e8
// state_merkleroot: dde529c57dd9924f169825c4415e4779c95f0e02c59af1d8b1f7a833520e5446
// tx_merkleroot: 9c3c8bbffa9e86ffd84711ad649edc4ae9b657c732028cb3f22acb6822c8a741
// receipt_merkleroot: 72cf29f67b3a97c4dabf832725d1ef0780bbc8cb8faf68e7c9c7a124461b622c
// bloom: 10200000410000000000000080000000000000000200000000010000040000000000000000000200000048000000000882048000080028000000000000200010000000080000013808000408000000200000000004401000080000000000000000000000020000200000000000000840010000000000040000000010000800000000000002000010064000000000000000000000010000180000004000002000120001000000200000004000000000000001020000000000002000021008000000000002400000002000100000000000000000000000001040000002000020000010a00000000000000010000000000010800000200000000200020000000000
// difficulty: 7656001252874407
// number: 12970000
// gas_limit: 29999567
// gas_used: 645288
// time: 1628233655
// extra: 457468657265756d50504c4e532f326d696e6572735f455533
// mix_digest: 6af002e55cd5e5c4f4e04c15ac48e9a2d2e88e364639b0713b70a77509caa556
// nonce: 8af5a4039d877336
// hash: 13049bb8cfd97fe2333829f06df37c569db68d42c23097fbac64f2c61471f281
// base_fee: 28690644740

// parent_hash: 13049bb8cfd97fe2333829f06df37c569db68d42c23097fbac64f2c61471f281
// uncle_hash: 1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347
// miner: ea674fdde714fd979de3edf0f56aa9716b898ec8
// state_merkleroot: 9f1c3e7aa02f252555f4de00f87f270779fbe6a1c90f3f43e6e0f420eead6c36
// tx_merkleroot: 8baa0ea7ef6acbc834e1e707df765e85b265f51ff238d6c940f526233bbb0aae
// receipt_merkleroot: 1164615a3f7f890089fc16b16fe3d4c7d7e9fac71debea357db154fdbec60411
// bloom: 70f15b52e5937b2910f025def3a1903bff1994d2be5235645331206abe92e0576c6f775a22cbbed0a059df8f863d2398ff0fbbc95d3dff2183c29b3463f5ec5f38bcf846c9be513cddf6b02e17d975fceda3f46c68e0bf156e27df6aca6f03827fe0ec0d239e4b209eaaffb0cb530b7bb04f8d11088b4e63dee04d9e9b7acd9ef60821253cbffa90b6780d69ffa65eff543d952965142c18aaab6676a85116b082dd70ab5fe9e5c25c7bcbd6e934cfb1f361efdf1683e1c06136cb4b3f1d61747356d6423a3b81e33467327e27af5b2df91d9323d50f92127ea467fe167ce676f9397a2eaad61561c5159a326d9a868d56f6b4c1e772e2fe067c8921526b74db
// difficulty: 7659740608477986
// number: 12970001
// gas_limit: 30000000
// gas_used: 29980167
// time: 1628233658
// extra: 65746865726d696e652d75732d6561737431
// mix_digest: 79c1383210280b431543525da4950695a902920e0ac3133f25f8ddf537514bb0
// nonce: 13d0d10002c6671e
// hash: ffcaf92863a2cf8bee2ac451a06c93a839403d9a384cbfa4df36d9cb59e028fe
// base_fee: 25258597453

static void verify_ethash(xbytes_t & left_bytes) {
    while (left_bytes.size() != 0) {
        RLP::DecodedItem item = RLP::decode(left_bytes);
        left_bytes = std::move(item.remainder);
        xeth_header_t header;
        {
            auto item_header = RLP::decode_once(item.decoded[0]);
            auto header_bytes = item_header.decoded[0];
            header.decode_rlp(header_bytes);
        }
        auto proofs_per_node = static_cast<uint64_t>(evm_common::fromBigEndian<u64>(item.decoded[item.decoded.size() - 1]));
        std::vector<double_node_with_merkle_proof> nodes;
        uint32_t nodes_size{64};
        uint32_t nodes_start_index{2};
        uint32_t proofs_start_index{2 + nodes_size * 2};
        for (size_t i = 0; i < nodes_size; i++) {
            double_node_with_merkle_proof node;
            node.dag_nodes.emplace_back(static_cast<h512>(item.decoded[nodes_start_index + 2 * i]));
            node.dag_nodes.emplace_back(static_cast<h512>(item.decoded[nodes_start_index + 2 * i + 1]));
            for (size_t j = 0; j < proofs_per_node; j++) {
                node.proof.emplace_back(static_cast<h128>(item.decoded[proofs_start_index + proofs_per_node * i + j]));
            }
            nodes.emplace_back(node);
        }
        EXPECT_TRUE(xethash_t::instance().verify_seal(header, nodes));
    }
}

TEST(ethash, ethash_1270000) {
    std::error_code ec;
    xbytes_t output_bytes = top::from_hex(relayer_hex_output_1270000, ec);
    EXPECT_EQ(ec.value(), 0);
    verify_ethash(output_bytes);
}

TEST(ethash, ethash_1270001) {
    std::error_code ec;
    xbytes_t output_bytes = top::from_hex(relayer_hex_output_1270001, ec);
    EXPECT_EQ(ec.value(), 0);
    verify_ethash(output_bytes);
}

TEST(ethash, ethash_batch) {
    std::error_code ec;
    xbytes_t left_bytes = top::from_hex(relayer_hex_output_batch, ec);
    EXPECT_EQ(ec.value(), 0);
    verify_ethash(left_bytes);
}

TEST_F(xcontract_fixture_t, test_init_and_sync) {
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
    EXPECT_EQ(ec.value(), 0);
    auto sync_param1 = top::from_hex(relayer_hex_output_1270000, ec);
    EXPECT_EQ(ec.value(), 0);
    auto sync_param2 = top::from_hex(relayer_hex_output_1270001, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.init(init_param));
    EXPECT_TRUE(contract.sync(sync_param1));
    EXPECT_TRUE(contract.sync(sync_param2));

    auto bytes12969999 = from_hex("749c4c2c82582cba3489bce142d6c776d50a5c18b3aeaf3cdc68b27650d0a6b6", ec);
    EXPECT_EQ(contract.get_effective_hash(12969999), static_cast<h256>(bytes12969999));
    auto bytes12970000 = from_hex("13049bb8cfd97fe2333829f06df37c569db68d42c23097fbac64f2c61471f281", ec);
    EXPECT_EQ(contract.get_effective_hash(12970000), static_cast<h256>(bytes12970000));
    auto bytes12970001 = from_hex("ffcaf92863a2cf8bee2ac451a06c93a839403d9a384cbfa4df36d9cb59e028fe", ec);
    EXPECT_EQ(contract.get_effective_hash(12970001), static_cast<h256>(bytes12970001));

    EXPECT_EQ(contract.get_last_hash(), static_cast<h256>(bytes12970001));
    EXPECT_EQ(12970001, contract.get_height());

    auto hashes = contract.get_hashes(12969999);
    EXPECT_EQ(hashes.size(), 1);
    EXPECT_TRUE(hashes.count(static_cast<h256>(bytes12969999)));
    hashes = contract.get_hashes(12970000);
    EXPECT_EQ(hashes.size(), 1);
    EXPECT_TRUE(hashes.count(static_cast<h256>(bytes12970000)));
    hashes = contract.get_hashes(12970001);
    EXPECT_EQ(hashes.size(), 1);
    EXPECT_TRUE(hashes.count(static_cast<h256>(bytes12970001)));

    xeth_header_info_t info;
    EXPECT_TRUE(contract.get_header_info(static_cast<h256>(bytes12969999), info));
    EXPECT_EQ(info.number, 12969999);
    EXPECT_EQ(info.difficult_sum, 7652263722236960);
    EXPECT_TRUE(contract.get_header_info(static_cast<h256>(bytes12970000), info));
    EXPECT_EQ(info.number, 12970000);
    EXPECT_EQ(info.difficult_sum, 7652263722236960 + 7656001252874407);
    EXPECT_EQ(info.parent_hash, static_cast<h256>(bytes12969999));
    EXPECT_TRUE(contract.get_header_info(static_cast<h256>(bytes12970001), info));
    EXPECT_EQ(info.number, 12970001);
    EXPECT_EQ(info.difficult_sum, 7652263722236960 + 7656001252874407 + 7659740608477986);
    EXPECT_EQ(info.parent_hash, static_cast<h256>(bytes12970000));
}

TEST_F(xcontract_fixture_t, test_double_init) {
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.init(init_param));
    EXPECT_FALSE(contract.init(init_param));
}

TEST_F(xcontract_fixture_t, test_error_init_param) {
    const char * hex_init_param = "f90210a0fc40ad4f64dd51d09e94d9b7f1136cdcaaf03fb9a75c32661228bd3212547107a01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794cf4d39b0edb0b69cd15f687fd45c8fc8eb687daea0be5f8f7c84539b81c621a029b3083c37e0b63dde1372545c38edb792fcb65883a056e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421a056e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421b90100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008302196d65834f24ec8084628f482199d883010a12846765746888676f312e31372e38856c696e7578a0d8f6c88a59b42eafc6bc6e048e257a415bbb17822d57566153b73a72666b297b880afe7b9f331361e7";
    std::error_code ec;
    auto init_param = top::from_hex(hex_init_param, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_FALSE(contract.init(init_param));
}

TEST_F(xcontract_fixture_t, test_sync_not_init) {
    std::error_code ec;
    auto sync_param = top::from_hex(relayer_hex_output_1270000, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_FALSE(contract.sync(sync_param));
}

TEST_F(xcontract_fixture_t, test_sync_get_parent_header_error) {
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
    EXPECT_EQ(ec.value(), 0);
    auto sync_param = top::from_hex(relayer_hex_output_1270001, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.init(init_param));
    EXPECT_FALSE(contract.sync(sync_param));
}

TEST_F(xcontract_fixture_t, test_verify_common_time) {
    xeth_header_t h1;
    xeth_header_t h2;
    h1.number = 10;
    h2.number = 11;
    EXPECT_FALSE(contract.verify(h1, h2, {}));
}

TEST_F(xcontract_fixture_t, test_verify_common_extra_size) {
    xeth_header_t h1;
    xeth_header_t h2;
    h1.number = 10;
    h2.number = 11;
    h1.time = 10;
    h2.time = 9;
    h2.extra = xbytes_t(64, 1);
    EXPECT_FALSE(contract.verify(h1, h2, {}));
}

TEST_F(xcontract_fixture_t, test_verify_common_gaslimit) {
    xeth_header_t h1;
    xeth_header_t h2;
    h1.number = 10;
    h2.number = 11;
    h2.gas_limit = UINT64_MAX;
    EXPECT_FALSE(contract.verify(h1, h2, {}));
}

TEST_F(xcontract_fixture_t, test_verify_common_gasused) {
    xeth_header_t h1;
    xeth_header_t h2;
    h1.number = 10;
    h2.number = 11;
    h2.gas_limit = 5000;
    h2.gas_used = 6000;
    EXPECT_FALSE(contract.verify(h1, h2, {}));
}

TEST_F(xcontract_fixture_t, test_is_confirmed) {
    const char * hash1_hex = "749c4c2c82582cba3489bce142d6c776d50a5c18b3aeaf3cdc68b27650d0a6b6";
    const char * hash2_hex = "13049bb8cfd97fe2333829f06df37c569db68d42c23097fbac64f2c61471f281";
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
    EXPECT_EQ(ec.value(), 0);
    auto sync_param = top::from_hex(relayer_hex_output_1270000, ec);
    EXPECT_EQ(ec.value(), 0);
    auto hash1 = top::from_hex(hash1_hex, ec);
    EXPECT_EQ(ec.value(), 0);
    auto hash2 = top::from_hex(hash2_hex, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.init(init_param));
    EXPECT_TRUE(contract.sync(sync_param));
    for (auto i = 12970001; i < 12970025; i++) {
        EXPECT_TRUE(contract.set_effective_hash(i, h256(i)));
        xeth_header_info_t info;
        info.parent_hash = h256(i - 1);
        info.number = i;
        EXPECT_TRUE(contract.set_header_info(h256(i), info));
    }
    EXPECT_TRUE(contract.set_last_hash(h256(12970024)));

    EXPECT_TRUE(contract.is_confirmed(12969999, hash1));
    EXPECT_FALSE(contract.is_confirmed(12970000, hash2));
}

static xeth_header_t create_header(h256 parent_hash, uint32_t number, uint32_t difficulty) {
    xeth_header_t header;
    header.parent_hash = parent_hash;
    header.uncle_hash = static_cast<evm_common::h256>(0);
    header.miner = static_cast<evm_common::eth::Address>(0);
    header.state_merkleroot = static_cast<evm_common::h256>(0);
    header.tx_merkleroot = static_cast<evm_common::h256>(0);
    header.receipt_merkleroot = static_cast<evm_common::h256>(0);
    header.bloom = static_cast<evm_common::eth::LogBloom>(0);
    header.mix_digest = static_cast<evm_common::h256>(0);
    header.nonce = static_cast<evm_common::h64>(0);
    header.difficulty = difficulty;
    header.number = number;
    header.gas_limit = 0;
    header.gas_used = 0;
    header.time = 0;
    header.base_fee = static_cast<evm_common::bigint>(0);
    return header;
}

TEST_F(xcontract_fixture_t, test_rebuild1) {
    auto h10 = create_header(static_cast<evm_common::h256>(0), 10, 10);
    auto h11 = create_header(h10.hash(), 11, 10);
    auto h12 = create_header(h11.hash(), 12, 10);
    auto h13 = create_header(h12.hash(), 13, 10);
    auto h14 = create_header(h13.hash(), 14, 10);
    auto h15 = create_header(h14.hash(), 15, 10);
    auto h12_fork = create_header(h11.hash(), 12, 5);
    auto h13_fork = create_header(h12_fork.hash(), 13, 100);

    contract.set_header_info(h10.hash(), xeth_header_info_t{10, static_cast<evm_common::h256>(0), 10});
    contract.set_header_info(h11.hash(), xeth_header_info_t{20, h10.hash(), 11});
    contract.set_header_info(h12.hash(), xeth_header_info_t{30, h11.hash(), 12});
    contract.set_header_info(h13.hash(), xeth_header_info_t{40, h12.hash(), 13});
    contract.set_header_info(h14.hash(), xeth_header_info_t{50, h13.hash(), 14});
    contract.set_header_info(h15.hash(), xeth_header_info_t{60, h14.hash(), 15});
    contract.set_header_info(h12_fork.hash(), xeth_header_info_t{15, h11.hash(), 12});
    contract.set_header_info(h13_fork.hash(), xeth_header_info_t{115, h12_fork.hash(), 13});
    EXPECT_TRUE(contract.set_effective_hash(10, h10.hash()));
    EXPECT_TRUE(contract.set_effective_hash(11, h11.hash()));
    EXPECT_TRUE(contract.set_effective_hash(12, h12.hash()));
    EXPECT_TRUE(contract.set_effective_hash(13, h13.hash()));
    EXPECT_TRUE(contract.set_effective_hash(14, h14.hash()));
    EXPECT_TRUE(contract.set_effective_hash(15, h15.hash()));
    EXPECT_TRUE(contract.set_last_hash(h15.hash()));
    EXPECT_TRUE(contract.rebuild(h13_fork, xeth_header_info_t{60, h14.hash(), 15}, xeth_header_info_t{115, h12_fork.hash(), 13}));

    EXPECT_EQ(contract.get_last_hash(), h13_fork.hash());
    EXPECT_EQ(contract.get_effective_hash(15), h256());
    EXPECT_EQ(contract.get_effective_hash(14), h256());
    EXPECT_EQ(contract.get_effective_hash(13), h13_fork.hash());
    EXPECT_EQ(contract.get_effective_hash(12), h12_fork.hash());
    EXPECT_EQ(contract.get_effective_hash(11), h11.hash());
    EXPECT_EQ(contract.get_effective_hash(10), h10.hash());
}

TEST_F(xcontract_fixture_t, test_rebuild2) {
    auto h10 = create_header(static_cast<evm_common::h256>(0), 10, 10);
    auto h11 = create_header(h10.hash(), 11, 10);
    auto h12 = create_header(h11.hash(), 12, 10);
    auto h13 = create_header(h12.hash(), 13, 10);
    auto h14 = create_header(h13.hash(), 14, 10);
    auto h15 = create_header(h14.hash(), 15, 10);
    auto h12_fork = create_header(h11.hash(), 12, 5);
    auto h13_fork = create_header(h12_fork.hash(), 13, 5);
    auto h14_fork = create_header(h13_fork.hash(), 14, 5);
    auto h15_fork = create_header(h14_fork.hash(), 15, 5);
    auto h16_fork = create_header(h15_fork.hash(), 16, 5);
    auto h17_fork = create_header(h16_fork.hash(), 17, 100);

    contract.set_header_info(h10.hash(), xeth_header_info_t{10, static_cast<evm_common::h256>(0), 10});
    contract.set_header_info(h11.hash(), xeth_header_info_t{20, h10.hash(), 11});
    contract.set_header_info(h12.hash(), xeth_header_info_t{30, h11.hash(), 12});
    contract.set_header_info(h13.hash(), xeth_header_info_t{40, h12.hash(), 13});
    contract.set_header_info(h14.hash(), xeth_header_info_t{50, h13.hash(), 14});
    contract.set_header_info(h15.hash(), xeth_header_info_t{60, h14.hash(), 15});
    contract.set_header_info(h12_fork.hash(), xeth_header_info_t{15, h11.hash(), 12});
    contract.set_header_info(h13_fork.hash(), xeth_header_info_t{20, h12_fork.hash(), 13});
    contract.set_header_info(h14_fork.hash(), xeth_header_info_t{25, h13_fork.hash(), 14});
    contract.set_header_info(h15_fork.hash(), xeth_header_info_t{30, h14_fork.hash(), 15});
    contract.set_header_info(h16_fork.hash(), xeth_header_info_t{35, h15_fork.hash(), 16});
    contract.set_header_info(h17_fork.hash(), xeth_header_info_t{135, h16_fork.hash(), 17});
    EXPECT_TRUE(contract.set_effective_hash(10, h10.hash()));
    EXPECT_TRUE(contract.set_effective_hash(11, h11.hash()));
    EXPECT_TRUE(contract.set_effective_hash(12, h12.hash()));
    EXPECT_TRUE(contract.set_effective_hash(13, h13.hash()));
    EXPECT_TRUE(contract.set_effective_hash(14, h14.hash()));
    EXPECT_TRUE(contract.set_effective_hash(15, h15.hash()));
    EXPECT_TRUE(contract.set_last_hash(h15.hash()));
    EXPECT_TRUE(contract.rebuild(h17_fork, xeth_header_info_t{60, h14.hash(), 15}, xeth_header_info_t{135, h16_fork.hash(), 17}));

    EXPECT_EQ(contract.get_last_hash(), h17_fork.hash());
    EXPECT_EQ(contract.get_effective_hash(17), h17_fork.hash());
    EXPECT_EQ(contract.get_effective_hash(16), h16_fork.hash());
    EXPECT_EQ(contract.get_effective_hash(15), h15_fork.hash());
    EXPECT_EQ(contract.get_effective_hash(14), h14_fork.hash());
    EXPECT_EQ(contract.get_effective_hash(13), h13_fork.hash());
    EXPECT_EQ(contract.get_effective_hash(12), h12_fork.hash());
    EXPECT_EQ(contract.get_effective_hash(11), h11.hash());
    EXPECT_EQ(contract.get_effective_hash(10), h10.hash());
}

TEST_F(xcontract_fixture_t, execute_input_with_error_account) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    context.caller.build_from("f8a1e199c49c2ae2682ecc5b4a8838b39bab1a39");
    EXPECT_FALSE(contract.execute({}, 0, context, false, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t, execute_input_empty) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    EXPECT_FALSE(contract.execute({}, 0, context, false, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t, execute_input_invalid) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::string str{relayer_hex_output_init_1269999_tx_data};
    str.insert(str.begin(), 'x');
    str.insert(str.begin(), '0');
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_output_init_1269999_tx_data, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_FALSE(contract.execute(init_param, 0, context, false, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t, execute_input_invalid_method_id) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::string str{relayer_hex_output_init_1269999_tx_data};
    str[0] = '5';
    std::error_code ec;
    auto init_param = top::from_hex(str, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_FALSE(contract.execute(init_param, 0, context, false, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t,  execute_method_id_extract_error) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.init(init_param));
    auto sync_param = top::from_hex(relayer_hex_output_sync_1270000_tx_data, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.execute(sync_param, 0, context, false, statectx_observer, output, err));
    std::string str{relayer_hex_output_get_height_tx_date};
    str = {str.begin(), str.begin() + 4};
    auto height_param = top::from_hex(str, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_FALSE(contract.execute(height_param, 0, context, true, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t, execute_init_extract_error1) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::string str{relayer_hex_output_init_1269999_tx_data};
    str = {str.begin(), str.begin() + 8};
    std::error_code ec;
    auto init_param = top::from_hex(str, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_FALSE(contract.execute(init_param, 0, context, false, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t, execute_init_static_error) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_output_init_1269999_tx_data, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_FALSE(contract.execute(init_param, 0, context, true, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t, execute_init_verify_error) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_output_init_1269999_tx_data, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_FALSE(contract.execute(init_param, 0, context, false, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t,  execute_sync_ok) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.init(init_param));

    auto sync_param = top::from_hex(relayer_hex_output_sync_1270000_tx_data, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.execute(sync_param, 0, context, false, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t, execute_sync_extract_error) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.init(init_param));

    std::string str{relayer_hex_output_sync_1270000_tx_data};
    str = {str.begin(), str.begin() + 8};
    auto sync_param = top::from_hex(str, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_FALSE(contract.execute(sync_param, 0, context, false, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t,  execute_sync_static_error) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.init(init_param));

    auto sync_param = top::from_hex(relayer_hex_output_sync_1270000_tx_data, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_FALSE(contract.execute(sync_param, 0, context, true, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t,  execute_sync_error) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.init(init_param));

    auto sync_param = top::from_hex(relayer_hex_output_sync_1270000_tx_data, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.execute(sync_param, 0, context, false, statectx_observer, output, err));
    EXPECT_FALSE(contract.execute(sync_param, 0, context, false, statectx_observer, output, err));
}

TEST_F(xcontract_fixture_t,  execute_get_height) {
    contract_runtime::evm::sys_contract_precompile_output output;
    contract_runtime::evm::sys_contract_precompile_error err;
    std::error_code ec;
    auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.init(init_param));
    auto sync_param = top::from_hex(relayer_hex_output_sync_1270000_tx_data, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.execute(sync_param, 0, context, false, statectx_observer, output, err));
    auto height_param = top::from_hex(relayer_hex_output_get_height_tx_date, ec);
    EXPECT_EQ(ec.value(), 0);
    EXPECT_TRUE(contract.execute(height_param, 0, context, true, statectx_observer, output, err));
    u256 h = evm_common::fromBigEndian<u256>(output.output);
    EXPECT_EQ(h, 12970000);
}

// TEST_F(xcontract_fixture_t,  execute_is_confirmed) {
//     contract_runtime::evm::sys_contract_precompile_output output;
//     contract_runtime::evm::sys_contract_precompile_error err;
//     std::error_code ec;
//     auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
//     EXPECT_EQ(ec.value(), 0);
//     EXPECT_TRUE(contract.init(init_param));
//     auto sync_param = top::from_hex(relayer_hex_output_sync_1270000_tx_data, ec);
//     EXPECT_EQ(ec.value(), 0);
//     EXPECT_TRUE(contract.execute(sync_param, 0, context, false, statectx_observer, output, err));
//     auto is_confirmed_param = top::from_hex(relayer_hex_output_is_confirmed_tx_data, ec);
//     EXPECT_EQ(ec.value(), 0);
//     EXPECT_TRUE(contract.execute(is_confirmed_param, 0, context, false, statectx_observer, output, err));
//     EXPECT_EQ(evm_common::fromBigEndian<u256>(output.output), 0);
// }

// TEST_F(xcontract_fixture_t,  execute_is_confirmed_extract_error) {
//     contract_runtime::evm::sys_contract_precompile_output output;
//     contract_runtime::evm::sys_contract_precompile_error err;
//     std::error_code ec;
//     auto init_param = top::from_hex(relayer_hex_init_12969999, ec);
//     EXPECT_EQ(ec.value(), 0);
//     EXPECT_TRUE(contract.init(init_param));
//     auto sync_param = top::from_hex(relayer_hex_output_sync_1270000_tx_data, ec);
//     EXPECT_EQ(ec.value(), 0);
//     EXPECT_TRUE(contract.execute(sync_param, 0, context, false, statectx_observer, output, err));

//     std::string str{relayer_hex_output_is_confirmed_tx_data};
//     str = {str.begin(), str.begin() + 70};
//     auto is_confirmed_param = top::from_hex(str, ec);
//     EXPECT_EQ(ec.value(), 0);
//     EXPECT_FALSE(contract.execute(is_confirmed_param, 0, context, false, statectx_observer, output, err));
//     EXPECT_EQ(evm_common::fromBigEndian<u256>(output.output), 0);
// }

TEST_F(xcontract_fixture_t, test_release) {
    // num = 40000
    for (auto i = 10000; i <= 60000; i++) {
        EXPECT_TRUE(contract.set_effective_hash(i, h256(i)));
    }
    // num = 500
    xeth_header_t header;
    xeth_header_info_t info;
    for (auto i = 59000; i <= 60000; i++) {
        EXPECT_TRUE(contract.set_header(h256(i), header));
        info.difficult_sum = i;
        info.parent_hash = h256(i - 1);
        info.number = i;
        EXPECT_TRUE(contract.set_header_info(h256(i), info));
        EXPECT_TRUE(contract.set_hashes(i, {h256(i)}));
    }
    contract.release(60000);
    for (auto i = 10000; i <= 20000; i++) {
        EXPECT_EQ(contract.get_effective_hash(i), h256());
    }
    for (auto i = 20001; i <= 60000; i++) {
        EXPECT_EQ(contract.get_effective_hash(i), h256(i));
    }
    for (auto i = 59000; i <= 59500; i++) {
        EXPECT_FALSE(contract.get_header_info(h256(i), info));
        EXPECT_FALSE(contract.get_header(h256(i), header));
        EXPECT_TRUE((contract.get_hashes(i)).empty());
    }
    for (auto i = 59501; i <= 60000; i++) {
        EXPECT_TRUE(contract.get_header(h256(i), header));
        EXPECT_TRUE(contract.get_header_info(h256(i), info));
        EXPECT_EQ(info.difficult_sum, i);
        EXPECT_EQ(info.number, i);
        EXPECT_EQ(info.parent_hash, h256(i-1));
        EXPECT_FALSE((contract.get_hashes(i)).empty());
    }
}

TEST_F(xcontract_fixture_t, test_extract_base32) {
    const char * text = "efd8beed749c4c2c82582cba3489bce142d6c776d50a5c18b3aeaf3cdc68b27650d0a6b6";

    std::error_code ec;
    auto bytes = top::from_hex(text, ec);
    evm_common::xabi_decoder_t abi_decoder = evm_common::xabi_decoder_t::build_from(xbytes_t{std::begin(bytes), std::end(bytes)}, ec);
    auto function_selector = abi_decoder.extract<evm_common::xfunction_selector_t>(ec);
    function_selector;
    auto hash_bytes = abi_decoder.decode_bytes(32, ec);
    h256 hash = static_cast<h256>(hash_bytes);
    EXPECT_EQ("749c4c2c82582cba3489bce142d6c776d50a5c18b3aeaf3cdc68b27650d0a6b6", hash.hex());
}

}
}
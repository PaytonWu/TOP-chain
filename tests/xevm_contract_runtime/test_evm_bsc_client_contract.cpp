#include "tests/xevm_contract_runtime/test_evm_eth_bridge_contract_fixture.h"
#include "xbasic/xhex.h"
#include "xcommon/rlp.h"
#include "xevm_common/xabi_decoder.h"
#include "xevm_common/xcrosschain/xvalidators_snapshot.h"
#include "xevm_contract_runtime/sys_contract/xevm_bsc_client_contract.h"

#define private public
#include "xevm_common/xcrosschain/xeth_header.h"
#include "xevm_contract_runtime/xevm_sys_contract_face.h"

namespace top {
namespace tests {

using namespace contract_runtime::evm::sys_contract;
using namespace evm_common;

class xbsc_contract_fixture_t : public testing::Test {
public:
    xbsc_contract_fixture_t() {
    }

    void init() {
        auto bstate = make_object_ptr<base::xvbstate_t>(
            evm_eth_bridge_contract_address.to_string(), (uint64_t)0, (uint64_t)0, std::string(), std::string(), (uint64_t)0, (uint32_t)0, (uint16_t)0);
        auto canvas = make_object_ptr<base::xvcanvas_t>();
        bstate->new_string_map_var(data::system_contract::XPROPERTY_HEADERS, canvas.get());
        bstate->new_string_map_var(data::system_contract::XPROPERTY_HEADERS_SUMMARY, canvas.get());
        bstate->new_string_map_var(data::system_contract::XPROPERTY_ALL_HASHES, canvas.get());
        bstate->new_string_map_var(data::system_contract::XPROPERTY_EFFECTIVE_HASHES, canvas.get());
        bstate->new_string_var(data::system_contract::XPROPERTY_LAST_HASH, canvas.get());
        auto bytes = (evm_common::h256(0)).asBytes();
        bstate->load_string_var(data::system_contract::XPROPERTY_LAST_HASH)->reset({bytes.begin(), bytes.end()}, canvas.get());
        contract_state = std::make_shared<data::xunit_bstate_t>(bstate.get(), bstate.get());
        statectx = top::make_unique<xmock_statectx_t>(contract_state);
        statectx_observer = make_observer<statectx::xstatectx_face_t>(statectx.get());
        context.address = common::xtop_eth_address::build_from("ff00000000000000000000000000000000000003");
        context.caller = common::xtop_eth_address::build_from("f8a1e199c49c2ae2682ecc5b4a8838b39bab1a38");
    }

    void SetUp() override {
        init();
    }

    void TearDown() override {
    }

    contract_runtime::evm::sys_contract::xevm_bsc_client_contract_t contract;
    data::xunitstate_ptr_t contract_state;
    std::unique_ptr<statectx::xstatectx_face_t> statectx;
    observer_ptr<statectx::xstatectx_face_t> statectx_observer;
    contract_runtime::evm::sys_contract_context context;
};

#include "tests/xevm_contract_runtime/test_evm_bsc_client_contract_data.cpp"

// TEST(test, test) {
//     auto rlp_bytes = from_hex(heco_sync_17276012_hex);
//     // auto decode_items = RLP::decode(rlp_bytes);
//     // printf("%lu, %lu\n", decode_items.decoded.size(), decode_items.remainder.size());

//     auto left_bytes = std::move(rlp_bytes);
//     while (left_bytes.size() != 0) {
//         RLP::DecodedItem item = RLP::decode(left_bytes);
//         auto decoded_size = item.decoded.size();
//         if (decoded_size < 4) {
//             xwarn("[xtop_evm_eth_bridge_contract::sync] rlp param error");
//             return;
//         }
//         left_bytes = std::move(item.remainder);
//         xeth_header_t header;
//         {
//             auto item_header = RLP::decode_once(item.decoded[0]);
//             auto header_bytes = item_header.decoded[0];
//             if (header.decode_rlp(header_bytes) == false) {
//                 xwarn("[xtop_evm_eth_bridge_contract::sync] decode header error");
//                 return;
//             }
//         }
//         header.print();

//         auto validator_num = static_cast<uint64_t>(evm_common::fromBigEndian<u64>(item.decoded[1]));
//         if (decoded_size < validator_num + 2 + 1) {
//             xwarn("[xtop_evm_eth_bridge_contract::sync] rlp param error");
//             return;
//         }

//         heco::xheco_snapshot_t snap;
//         snap.number = static_cast<uint64_t>(header.number - 1);
//         snap.hash = header.parent_hash;
//         for (uint64_t i = 0; i < validator_num; ++i) {
//             snap.validators.insert(item.decoded[i + 2]);
//             printf("validator: %s\n", to_hex(item.decoded[i + 2]).c_str());
//         }

//         auto recent_num = static_cast<uint64_t>(evm_common::fromBigEndian<u64>(item.decoded[1 + validator_num + 1]));
//         if (decoded_size < validator_num + 2 + 1 + recent_num) {
//             xwarn("[xtop_evm_eth_bridge_contract::sync] rlp param error");
//             return;
//         }

//         for (uint64_t i = 0; i < recent_num; ++i) {
//             uint64_t k = static_cast<uint64_t>(evm_common::fromBigEndian<u64>(item.decoded[1 + validator_num + 1 + 1 + i * 2]));
//             snap.recents[k] = item.decoded[1 + validator_num + 1 + 1 + i * 2 + 1];
//             printf("recents: %lu, %s\n", k, to_hex(item.decoded[1 + validator_num + 1 + 1 + i * 2 + 1]).c_str());
//         }
//         snap.print();
//     }
// }

// TODO: delete
// TEST_F(xbsc_contract_fixture_t, test_bsc_init) {
//     auto bytes = top::from_hex(bsc_init_20250000_hex);
//     EXPECT_TRUE(contract.init(bytes, contract_state));
// }

TEST_F(xbsc_contract_fixture_t, test_parse_validators) {
    const char * header_hex = "f90403a0c18af239fc81de948d4f7785b1ea8c70eaa23283a1ca29f7bfbd164f91d37201a01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794be807dddb074639cd9fa61b47676c064fc50d62ca0cce0dfc134ef9611ed22535b05e1ff39ada7eef0613f62311b11e8af31243225a034000ae64d1693c21abfdce1af136c2b2ebfea48626add1a911a08642830d532a0bc20983deafc6035dd81cf7fea6aff00d6a06e2b2c1db78236e3b97123a9c849b9010000a222890b00c03030240001920c04e014000010e610a5a444300000281045a24080144201801012efe020d190c04032007900d106b00d60e10320402b3f00801e04882055301818013000080501502162100c000cc408908a04320298080000409014ec0e472018904b80c40508892288604042ce84848682240018880004297a04100124090d4000c21210285608a78c651407000a0409011009730027882002394100110952c03610a0218a0a880472000912090083a51208200005620828602c691208064d2722a048001a00d0c0b2429106e90170b109414b831000e0a12070542804434960d1c4848100ca920140027461a1000854200201380008309102840134fd908404c54a9b834ef0368462f07be6b90205d88301010b846765746888676f312e31332e34856c696e75780000005d43d2fd2465176c461afb316ebc773c61faee85a6515daa295e26495cef6f69dfa69911d9d8e4f3bbadb89b2b3a6c089311b478bf629c29d790a7a6db3fc1b92d4c407bbe49438ed859fe965b140dcf1aab71a93f349bbafec1551819b8be1efea2fc46ca749aa161dd481a114a2e761c554b641742c973867899d3685b1ded8013785d6623cc18d214320b6bb6475970f657164e5b75689b64b7fd1fa275f334f28e1872b61c6014342d914470ec7ac2975be345796c2b7ae2f5b9e386cd1b50a4550696d957cb4900f03a8b6c8fd93d6f4cea42bbb345dbc6f0dfdb5bec739f8ccdafcc39f3c7d6ebf637c9151673cbc36b88a6f79b60359f141df90a0c745125b131caaffd12aacf6a8119f7e11623b5a43da638e91f669a130fac0e15a038eedfc68ba3c35c73fed5be4a07afb5be807dddb074639cd9fa61b47676c064fc50d62ce2d3a739effcd3a99387d015e260eefac72ebea1e9ae3261a475a27bb1028f140bc2a7c843318afdea0a6e3c511bbd10f4519ece37dc24887e11b55dee226379db83cffc681495730c11fdde79ba4c0cef0274e31810c9df02f98fafde0f841f4e66a1cd5f0b40e8f4d4c857f4ea521ab99410c2a3d370e1f9f38fc5fa195eb3ed7707fc2f3fd6541d5041cbd30ee63f1aced286eefe989742716965d9927ca0d09e635b01a00000000000000000000000000000000000000000000000000000000000000000880000000000000000";
    auto bytes = top::from_hex(header_hex);
    xeth_header_t header;
    header.decode_rlp(bytes);
    xvalidators_snapshot_t snap;
    EXPECT_TRUE(snap.init_with_epoch(header));
    for (auto v : snap.validators) {
        printf("%s\n", to_hex(v).c_str());
    }
}

const char * init_error_hex = "0xf90403a004004c5e9a630b79eb53a9330f1db5516e696b37939e6c16bc6d576a9c2f4e2ca01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347947ae2f5b9e386cd1b50a4550696d957cb4900f03aa06f05252cf338cb174ef46bf08694ea5840e877f6175ca83da84884d9df7d9eb7a05f7ebfbc2e8173aca9381d7a2be5e0f27e5dd5533fbb7c8f58c8478ee0db99afa01cb53ebec99f61785f66f505e1125c681cada89cf1ff222d2d3e4b9cfcf0dcabb901004aa002011440135032d4003880349408010c030ae0448000e013040844180830438297084c8100050a98000829235020221420401a8680a2630e104f326002005000168845413800093c808980280aa86298010953400597225606519881030249ac28a54bfa0033844320d18c881d212b08002408b2448a86040a1952a101285c000321029315622888368209518810056d4c8b140205886014105902df60698380c16030808471222841c11aa04600442c80614784000506ae18222602682001300c0622420142404822401a241e40266033ec0140431725084e4a10dbb000c0511523a0c188e00382209809158800212811089ca6824812c1103181a009950284016ea1d084084fda76836c411284639fd11db90205d983010112846765746889676f312e31372e3133856c696e757800005b7663b52465176c461afb316ebc773c61faee85a6515daa295e26495cef6f69dfa69911d9d8e4f3bbadb89b2d4c407bbe49438ed859fe965b140dcf1aab71a93f349bbafec1551819b8be1efea2fc46ca749aa161dd481a114a2e761c554b641742c973867899d3685b1ded8013785d6623cc18d214320b6bb6475970f657164e5b75689b64b7fd1fa275f334f28e1872b61c6014342d914470ec7ac2975be345796c2b7ae2f5b9e386cd1b50a4550696d957cb4900f03a8b6c8fd93d6f4cea42bbb345dbc6f0dfdb5bec73a6f79b60359f141df90a0c745125b131caaffd12ac0e15a038eedfc68ba3c35c73fed5be4a07afb5b218c5d6af1f979ac42bc68d98a5a0d796c6ab01b4dd66d7c2c7e57f628210187192fb89d4b99dd4be807dddb074639cd9fa61b47676c064fc50d62ccc8e6d00c17eb431350c6c50d8b8f05176b90b11d1d6bf74282782b0b3eb1413c901d6ecf02e8e28e9ae3261a475a27bb1028f140bc2a7c843318afdea0a6e3c511bbd10f4519ece37dc24887e11b55dee226379db83cffc681495730c11fdde79ba4c0cef0274e31810c9df02f98fafde0f841f4e66a1cdd539b90275d3acaf234ebb7c6c4255d050f0e106b5df0abc8ec502c69c41c8c7462ea969f2c73ff563111d1439634192c240c6e3b5219939721b3892c2a7423000a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea0084568f6edef5dabf3411d8b9111f8223c698b2b35fc1f07f21a8df8ca0d0956a01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347948b6c8fd93d6f4cea42bbb345dbc6f0dfdb5bec73a0872650568946f7b2038a3fdede4b429f877ce0fa7da802cea11b0bed9244fce5a019dcbc248159cdc168f8bfc3dd762b670c828bbdebd63977c778f9b3513faf44a0a9f83e19f3470bab2e071515088f0f189817f20401dddd18fbd889759e7029d7b9010040b2074b80de1a140b0438c491926008b64a235151446480745111c50487c53a020894088410151e82298e0084611000210084000aa640a0664c062a182e2a080c048028800ba249019304d9c3a102bc661d0009434428a323f61029a8b0042c02042db20ee330434629ca1080088f130a8000400068de4d5046569134c10ca210040b260888920047a19f11214450089d762485540e02ea2d3a407000f03020120025421a401018664526da4e91542048f480582b22005b5cf8090b6120000421206c070094524206222049000212022c7221a011c800119286082e400cf140091a1222a453a46b230ac2902b4020c04ab0043a180a92dd024a1f324ab58a900284016ea1d18408582a4f837c6b0984639fd120b861d683010112846765746886676f312e3139856c696e757800000000005b7663b5ceb94a368df9745376169819aed6854589878eb45818e78715676d1576aed6a72ebf594d74596b1c606855cce31ed1d43bfe2176ae8c48197553924dfc6287bb00a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea07a28cfeb30370f061445f01d532e54176f947b5e6e8b4618a05615159945bc6aa01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794a6f79b60359f141df90a0c745125b131caaffd12a08a99982afb361c30f6057ee604d6a8973014f37c265a1c0e0e63d116dc8cf674a044f01f6a940f3088b413025712e94287274542b15bbad0625ea209fa4dcb18ffa0b7ea2d73cc3a563319cd10b2813c17cbcbc1259112f11967b0b4767278a1436bb90100c8308290c7d018102224d404b01ae8203100022080212400201000009100b03004002444b482100e4304202110008422a5021142000080044800422e00602a11149400424201000591221428e68001a0223ea299885248c295063a4888100985000030280ac200111205000480320c44aa01886088c085c06066f25888a70c0114c888a517050ba8009014a40057500a0475840554062b8801092064011409260a82450b190a1032ab8c40a93bce4cc0482200a000040305904060082182286a65020d0b001202624002244b6030121121d32dec211840198102108a214ef0a800385c0905d380488b02888d414a000050a0041c0046016a09020e1208a042000284016ea1d284084fd226839ac16d84639fd123b861d883010112846765746888676f312e31392e32856c696e75780000005b7663b5c849b541ab8ecb37db64630ca3c6851f63e70014f61b724386dc38aa2543be1d0420e6dea4fb039066330e50699f4d7319f60ef9546dbf5249a2fd606fe76a0e01a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea0d88c9f59017e6b76f9086d8a22c1b10997c394974d9ad963a568214fc4b768d1a01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794ac0e15a038eedfc68ba3c35c73fed5be4a07afb5a032938bf8078064107e010b320ca7d66f5019a85330d8b2f8b946c73a2f7758f5a07fd18ff3d5678b41c91cc77c00005ce79edadaa3bbe2564265624efcd5127f75a0d7b5b36e2f318c6662556c28ff0c2b6157cd8d884b1ded46828feb60d153e1f1b901005270ce100444f694861e6244825a4484500013569004b4097854b16006000032c6221540d60714103964b072d1140410f811901814ea200243f8310ab42407800820149210b00120011a010b80a210a4a4180139434c2883b44c030c8809011080861ca08e820c60d803b848c4020c728f400af188c057148924735140859e009110f1283811020905e11c014c68190924240c9517073728091008440a8a88222302804110a900b119067020069a4308d40a080004042885020020c822020049a0000612018c0146210042198266722022d02320414800118618840b01abf121003202160041804c4100b4514745200040802e0068098a780200183303b744300284016ea1d384084782558376019084639fd126b861d983010112846765746889676f312e31372e3133856c696e757800005b7663b5323e80a2e03007c94b5965e69a102ab6921913ef831245ba33681804d3b2ce03660546277036892ce3609f06a7e35c0f726f5c82cb8b03e47cb6504a743e844a01a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea0cd3e35664d2c8a73fbe77cf6f8036763d75a96b63fa539a393cecce05ab4ee16a01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794b218c5d6af1f979ac42bc68d98a5a0d796c6ab01a0574870e05d7a85d847b82b86a81735f7d32aff5b8d11697d173f358196aaffc1a05a0d61cc0df65887ab237c7c0884c0ffd473021fa6830621f825293f05741f0ba01dd653ff9bd8487d22274d8eb1dfe9fbc403eecc2798296104c7b9ac88e40367b9010082720b009f080a14024c10b5a402108090492e4c1cd4a18022120a092040c53d4002a6812c081000f3e88053240060731800444204a00223722cf10e882402088a08274502160004014c9438a12928256819a68b415748a3d7040a00aaa22400080018268a030d70212024048858599bae0442c084c887200065a05b008d08c1867101ba04b019ec45a31140001758283074458516b0ac183530c8416893bc3026801400110002484840304913d645baee82402337ba250121f012182404188a018100070006c362491501180482128420162025d94444179603306e162ae44140500a20ac41487a110810880581000508840c28690592e80200336280e5489c0284016ea1d484083f3ad4837fc47e84639fd129b861d883010112846765746888676f312e31392e32856c696e75780000005b7663b57d995f67870ca39809195a97d1b6725afd7675ac13de9aa0075bc0b362a06a4474337e12d62709df28b2bd83813d901f88ea6ebecb759db4b6854375b200573301a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea0188382c8555fa1aaf257c4055f4ede55192cbf82009df72d6ce96d9533394890a01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794b4dd66d7c2c7e57f628210187192fb89d4b99dd4a0c2b96414c2c110af9316832f1c491952f4be5425d5f96982850604186a2266e9a04b9d40b71f851c0b9f42cce80676c2bab39b8c09dd18ca39a70620400e8bcbe6a0f4940e144bd9d46fd5cbab8c5e63f1730075aca17e8a68ccf7cf46840b23250db90100c02a3781a3cd1e1122e4a8ae801f91b01000825a504724087a3523061012c1380412e55e9694121162348000c48108d418092990ab345a104709435e24a01b02d02034504ea282852d3270ac83a000f239db115a6a4a388220ed0912e8506f00238b09a14aa709829c5474b242042cb18f2082c944cb4e440025e23114894c0510840b24ea5104638da83c039c410a1c253f2d0554062738a104b368008604aea712c041a410001748894e4132ae70dc4210020a118a0815134d346d2513c20a3352240a46d01143dc0642bd51431a6d2115352181438558a611524b5108e8a5addba06165d18cc8f11026248f2946c231984c091c0cb56e0335fb2668a028340284016ea1d5840836fb9b83e0e05a84639fd12cb861d983010112846765746889676f312e31372e3133856c696e757800005b7663b55a4527d7d43f88487798555643364d3376d9b94a31826498304cd8633f72faa96edec4c2471c01dab3e7acefaa8cf2718bb5fc358ff758c986a982853fbd7e9b00a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea0211a0aa45ddc02992a14df5914307c2541b275571d1fb9b1e70f14d6328461dba01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794be807dddb074639cd9fa61b47676c064fc50d62ca00c72c47f58cdbc93d96c0de42170fcd493a6c8f52dbf2a48007a056d98355202a021f2b3fcea5f7aaa8f20e33ba67e2cf9443cef9c677b1539e2647882e2ab452ca0e9b86284169565b52f7992bd854c166fa4b8502913365b2cea7c8c2690f6834db90100c56412220b1836164a55846ecc701100600c0718545235003035a425b113917a6114d64881801000c8159030e40052214200914888171a20464cd94b32af3200a483a2c030d1e6614d3029c9a1a028e430d92448135028c214b56b42e8a8da44220806a54baa2e953c0b19009142ac802f1b08ea250135468827013800eb48401182c22c015268a2e2bc96d0dc40be41042f6d86142302ac3309be5900a03028be8220401c91b0100d00cb7142e00e50166c1805040a900522ec00bfe242000000862502081c4fc31000520901425641ad402124037f505386421d4e100ae8a8297875c90d77d26821021042cba7218409800ec889d8b01802020a0201ed58aa0284016ea1d684083f329583908df484639fd12fb861d983010112846765746889676f312e31372e3133856c696e757800005b7663b55a3b2bd95822c241ee047708ae9ca2c605eb36a16fcd1a971443b1255ea98ee216086a9e92bb52848f99ce0afcd5485bde47075e64778c2f66a949f0d22504df01a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea0656268cd8e612dbe4fd3eb4d546cbc3b71e57ac396c5b9b975488e7a1236f86da01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794cc8e6d00c17eb431350c6c50d8b8f05176b90b11a01f8fde897379b878743526153c0af60b3c1915b89c339f7bd7a35523bd33ab1ba079140c5af546f5f7e9f8765fb33b3d44d89a440e7682d87223ecc3e61c0676bba0a0a00bddb60fa91d4d17b97f9b68f497bbb44a7c61646e19f5e49cfb48af76bdb901008624073c1e0012304680502c8802212f710404041025a0042111000c010100400000456a402130502314885180805692a8ac0000882800145210610801a500063094231810002981452a082a89084d382030300108c628a0a43d040c8c000a2d801032341a960c0241000100b8885aa10822604485341c000664d61300314c04110042000c01800800e015130102181222259f051480062c2912e0410d068922264290faf88ba818200434a262ee2348e5ca4025030411591a117103201a212821120442101a005e0c400809581012102101410c406427319606b092a42ae821c01000c00109014207411210050400025ac80c405848c978020b801680e14a8f0284016ea1d7840836f3648359aa1884639fd132b861d983010112846765746889676f312e31372e3133856c696e757800005b7663b53ee87f6349403fe40204bc647cd517aab53651cdc29e5f69e811c4f176f3b28e6d801e90dddc695a8e8e531c5d2406c86e930592ea1361c32dd2d167c85afdf600a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea0332078fc0d3bd8e4d9bbaa6251a03677ececf98b37d0ac0a182eed90d251164ca01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794ce2fd7544e0b2cc94692d4a704debef7bcb61328a034d2ef3160a06d3ed862bda817d1b40e40e9210c650e3b4890e5b9f825ac5db0a00173f511d8856e3297fb80a051f2a991138ff57f482ad8cc467d2412ca0e4656a03b6d792432099652a390fcf9b3c6d1db1046044ab49aa2689e3c90b28a13320db9010060751f40c74c165a42053025a0931518a0235bd2bcc165c02c518112980c3fd024a24560f48c5240a3260f2a345d8f20aa830ab9a87b18335f8c23d69d367948b03220a30120876b6135156e99a360b8709af22914545ebac43666aaca0b2c4253183c314f2a7e21036f39e584208c8e4f6560f1a1e8663c0e240e19143d6f7f1459901046cb717007a09e5524e6dc21942f45a59e3b430c11984b4147825a2327a743f51deff4327e90542797cf4200484470e9e19b0f4579182ba5600629283124bd060199316607c9b5ef86427b3260669b4685293d18dec010cf3daae9211039f70b9f9d80d95b71741201c8818650889e044b1863e1866a0f4283a94a160284016ea1d884083f2a5683a4a32884639fd135b861d983010112846765746889676f312e31372e3133856c696e757800005b7663b5706db41fb5cecd664ec85f66a718966a8cb423622390c8a0e87a83ae361e911c6bdc2743193c954726e45dc445b1c5a3fc51614d8a6c0decff75e8269c2544c200a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea02f601b318ec4cc1de09aca45a409c235c60803a2983c7b740f1a825d36a5bb0ea01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794d1d6bf74282782b0b3eb1413c901d6ecf02e8e28a0feb91adbc056648743b59e5f46d9f4a6c2cdb652f23999da4fd1388d875442aba0148fabbfff39d0bb08f5efa897790057890688c27b07381ea21993773be3e574a00f13f82271037170b0939fb870890b163f233b80ca4e4d7bcee0dd059c1e77c7b90100482003900204439ac0041342ca7002408808226c1c168d002015008400a09164030006e18ce0141047b8a2f08066228085944080827aca00560ef10010a44a880210ec008000c5938130d10cad9015b4e01237e1025e20b2807400088e194f01004519281fa644620205815200000cab1ab00011a40c34140825a23964034610118b20000e01202804a4108080c1010064240401c5400a2eec05014911cd6a3222030e504608f0137c06241322db2100430022010082b50904403131614a610927120006100340435083020dc8a23a0026652b0a22c0cc10a259a042081aec211c1006110861d0504d0430008101040842182018680a844a069392020020489c0284016ea1d9840836eb2d83892bd484639fd138b861d883010112846765746888676f312e31392e32856c696e75780000005b7663b5c2df77037264db29bb3198dd7c518f11e4adac7dbea2bfd8784b874cbe0b9839039eacec0e2ba7641dad877de2043861d9562ebec29373b988c2b2ae74505e3f01a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea0bb9615180b11613d317044cc7e5f1b1473a2cc0541d5b36a42ab694a45a21adca01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794e9ae3261a475a27bb1028f140bc2a7c843318afda06f7c5ddc54c1be027726e976042e00a04d551e9e3a8bb7011a11c3732fc16ff2a048c8c4de185849c03a5ff3ed8c630a42ff5ec1076275ecbde98213410e19a6d6a0255cba0529c2c9fa0d387086173489959b38524073822d50110fc4f04740e1d7b9010022214f016d0c1230522d001a81d280407018005cdc31810215d48820814101c00510544206228044d3b5845080860d7208e28ad00c3aa28a7b0c15424664e2029282a4020a292801412908c9c1bd03b2757d5048135420f3a48508008c20904080085b346b02a723a009190049029d851a2b007085880e8050665030000544845261b04e76590c6924e61e88a2d70008062d95c55e042c2c281992c1809a383aab8800001d8ad07a6a003239d3ca4142441e6920220207814c207080265a906821404422000a844a410042290a765602394c614bc589e819238104e6883fe8a3c0bc311503259469011c30000560002742852818436b4558828a488612f5589c0284016ea1da84083f2217837e6d4384639fd13bb861d983010112846765746889676f312e31372e3133856c696e757800005b7663b5a3fbb1383e889a9547b8b3d191de304707a35776593a974d3c8247aacfa2cc683482ce53a13996a036d5991b993c0cd5c3f0b047bd59c0c1e28c28f19ff83db401a00000000000000000000000000000000000000000000000000000000000000000880000000000000000f9025ea013dbf66808dcb4d75f4c441c314327ef20d98bd57cd3b1a6323cca0969aec72fa01dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d4934794ef0274e31810c9df02f98fafde0f841f4e66a1cda030d3b7e629a22f8f0644337fc7f6b597d0a09feb7b035c84ca8374997ed8bc0ba02bb37cab13137ebb70b85d69c6f6d4ead9f103878d8c130f3f6df0f172300b81a0eea403aabc4e4606157333a91a0767ea51a10b4fd82c8a133f3ce744a0c2c5c9b901006f24021445009730500e28d48006110050143206192c844020140ac82810813715020660e42b1018031014510002c250a00a1000420040305028030e00ec3a888a00035e04110a4597708088808840272818cc0f15540ea291a41602cc000400da091a2daaca2410200822cee001a8258e067042848226020425417040174500141011000ca1511806a096400400c8b306740405c622066839082046001a0a21ab0220012709005862403540628b0164a20600804ca0014360d9200c2402d829a1804822046040424402100310c2320826504020410c111167a3046b804ae02118503c4c49015840010410828112030048084c28c9cb4c481466836c002000040284016ea1db84084761388387781084639fd13eb861d883010112846765746888676f312e31382e32856c696e75780000005b7663b57c4fa9f941052ca2e7d1e1f07fdff9478bd9782e5849494beaa701a4a580e7c528f13d2bdac116022a2fe6adb66f077e2f560cbe511dc8342c4c7a984370552200a00000000000000000000000000000000000000000000000000000000000000000880000000000000000";

TEST_F(xbsc_contract_fixture_t, test_init_error) {
    const char * extra_hex = "0xd983010112846765746889676f312e31372e3133856c696e757800005b7663b50bac492386862ad3df4b666bc096b0505bb694da2465176c461afb316ebc773c61faee85a6515daa295e26495cef6f69dfa69911d9d8e4f3bbadb89b2d4c407bbe49438ed859fe965b140dcf1aab71a93f349bbafec1551819b8be1efea2fc46ca749aa161dd481a114a2e761c554b641742c973867899d3685b1ded8013785d6623cc18d214320b6bb6475972b61c6014342d914470ec7ac2975be345796c2b73564052d8e469ed0721c4e53379dc3c912289307ae2f5b9e386cd1b50a4550696d957cb4900f03a8b6c8fd93d6f4cea42bbb345dbc6f0dfdb5bec73a6f79b60359f141df90a0c745125b131caaffd12ac0e15a038eedfc68ba3c35c73fed5be4a07afb5b218c5d6af1f979ac42bc68d98a5a0d796c6ab01b4dd66d7c2c7e57f628210187192fb89d4b99dd4be807dddb074639cd9fa61b47676c064fc50d62ccc8e6d00c17eb431350c6c50d8b8f05176b90b11ce2fd7544e0b2cc94692d4a704debef7bcb61328d1d6bf74282782b0b3eb1413c901d6ecf02e8e28e9ae3261a475a27bb1028f140bc2a7c843318afdef0274e31810c9df02f98fafde0f841f4e66a1cde65173e67ad6055ed95d06a951e68b409809834183e59c65399bd74e50598d2c2ec062ff5cbf618ed049280624b90132c557042c7d5df8ba58a29564ef1980b901";
    auto extra_bytes = from_hex(std::string{extra_hex});
    auto init_error = from_hex(init_error_hex);

    auto item = RLP::decode_once(init_error);
    xeth_header_t header;
    assert(header.decode_rlp(item.decoded[0]));
    header.extra = extra_bytes;
    auto b = header.encode_rlp();
    b += init_error;
    EXPECT_TRUE(contract.init(b, contract_state));
}

}
}
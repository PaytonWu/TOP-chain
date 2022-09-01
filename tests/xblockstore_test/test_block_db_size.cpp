#include "gtest/gtest.h"

#include "xbase/xcontext.h"
#include "xbase/xdata.h"
#include "xbase/xobject.h"
#include "xbase/xmem.h"
#include "xbase/xcontext.h"
// TODO(jimmy) #include "xbase/xvledger.h"

#include "xdata/xdatautil.h"
#include "xdata/xemptyblock.h"
#include "xdata/xblocktool.h"
#include "xdata/xlightunit.h"
#include "xmbus/xevent_store.h"
#include "xmbus/xmessage_bus.h"
#include "xmbus/xbase_sync_event_monitor.hpp"
#include "xmetrics/xmetrics.h"

// #include "test_blockmock.hpp"
#include "xverifier/xtx_verifier.h"
#include "xblockstore/xblockstore_face.h"
#include "tests/mock/xvchain_creator.hpp"
#include "tests/mock/xdatamock_table.hpp"
#include "test_common.hpp"

using namespace top;
using namespace top::base;
using namespace top::mbus;
using namespace top::store;
using namespace top::data;
using namespace top::xverifier;
using namespace top::mock;
using namespace top::metrics;

class test_block_db_size : public testing::Test {
protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};
TEST_F(test_block_db_size, table_unit_size) {
// before optimize
// table = Ta0000@1 height=0 size = 213 object:213 input:0 output:0 offdata: 0
// table = Ta0000@1 height=1 size = 5236 object:1167 input:1578 output:154 offdata: 2337
// table = Ta0000@1 height=2 size = 5389 object:1195 input:1578 output:139 offdata: 2477
// table = Ta0000@1 height=3 size = 5564 object:1238 input:1578 output:139 offdata: 2609
// table = Ta0000@1 height=4 size = 5555 object:1229 input:1578 output:139 offdata: 2609
// table = Ta0000@1 height=5 size = 5556 object:1230 input:1578 output:139 offdata: 2609
// unit = T00000LXutXVNk3Kakvr8di6mcRWXEWrayCPEsSk height=0 size = 1087 header:90 qcert:145 input:135 output:86 input_res:358 output_res:180 binlog:102
// unit = T00000LXutXVNk3Kakvr8di6mcRWXEWrayCPEsSk height=1 size = 591 header:122 qcert:154 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LXutXVNk3Kakvr8di6mcRWXEWrayCPEsSk height=2 size = 627 header:122 qcert:156 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LXutXVNk3Kakvr8di6mcRWXEWrayCPEsSk height=3 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LXutXVNk3Kakvr8di6mcRWXEWrayCPEsSk height=4 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LXutXVNk3Kakvr8di6mcRWXEWrayCPEsSk height=5 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LXHWbydhyTPP4NTcHV6DAFjmXgASm4M4Cg height=0 size = 1049 header:90 qcert:145 input:135 output:86 input_res:358 output_res:180 binlog:102
// unit = T00000LXHWbydhyTPP4NTcHV6DAFjmXgASm4M4Cg height=1 size = 591 header:122 qcert:154 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LXHWbydhyTPP4NTcHV6DAFjmXgASm4M4Cg height=2 size = 627 header:122 qcert:156 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LXHWbydhyTPP4NTcHV6DAFjmXgASm4M4Cg height=3 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LXHWbydhyTPP4NTcHV6DAFjmXgASm4M4Cg height=4 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LXHWbydhyTPP4NTcHV6DAFjmXgASm4M4Cg height=5 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000Li6nDAcBokiREp3V1t6AQRB4P8k8mBwReC height=0 size = 1049 header:90 qcert:145 input:135 output:86 input_res:358 output_res:180 binlog:102
// unit = T00000Li6nDAcBokiREp3V1t6AQRB4P8k8mBwReC height=1 size = 591 header:122 qcert:154 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000Li6nDAcBokiREp3V1t6AQRB4P8k8mBwReC height=2 size = 627 header:122 qcert:156 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000Li6nDAcBokiREp3V1t6AQRB4P8k8mBwReC height=3 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000Li6nDAcBokiREp3V1t6AQRB4P8k8mBwReC height=4 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000Li6nDAcBokiREp3V1t6AQRB4P8k8mBwReC height=5 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LKbV3kW2VPBFVBAqAZw3S7GmNebb8AqQTK height=0 size = 1048 header:90 qcert:145 input:135 output:86 input_res:358 output_res:180 binlog:102
// unit = T00000LKbV3kW2VPBFVBAqAZw3S7GmNebb8AqQTK height=1 size = 591 header:122 qcert:154 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LKbV3kW2VPBFVBAqAZw3S7GmNebb8AqQTK height=2 size = 627 header:122 qcert:156 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LKbV3kW2VPBFVBAqAZw3S7GmNebb8AqQTK height=3 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LKbV3kW2VPBFVBAqAZw3S7GmNebb8AqQTK height=4 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// unit = T00000LKbV3kW2VPBFVBAqAZw3S7GmNebb8AqQTK height=5 size = 659 header:122 qcert:188 input:30 output:82 input_res:0 output_res:145 binlog:67
// db key size = 72544
// db value size = 319737
// hash_calc_count = 328

    mock::xvchain_creator creator(true);
    base::xvblockstore_t* blockstore = creator.get_blockstore();

    xhashtest_t::hash_calc_count = 0;
    xhashtest_t::print_hash_calc = false;
    uint64_t max_block_height = 5;
    mock::xdatamock_table mocktable(1, 4);
    mocktable.genrate_table_chain(max_block_height, blockstore);
    const std::vector<xblock_ptr_t> & tableblocks = mocktable.get_history_tables();
    xassert(tableblocks.size() == max_block_height + 1);


    xhashtest_t::print_hash_calc = false;
    for (auto & block : tableblocks) {
        ASSERT_TRUE(blockstore->store_block(mocktable, block.get()));

        std::string block_object_bin;
        block->serialize_to_string(block_object_bin);
        size_t t_size = block_object_bin.size() + block->get_input()->get_resources_data().size() + block->get_output()->get_resources_data().size() + block->get_output_offdata().size();
        std::cout << "table = " <<  block->get_account() << " height=" << block->get_height() << " size = " << t_size
        << " object:" << block_object_bin.size() 
        << " input:" << block->get_input()->get_resources_data().size() 
        << " output:" << block->get_output()->get_resources_data().size() 
        << " offdata: " << block->get_output_offdata().size() << std::endl;
    }

    for (auto & datamock_unit : mocktable.get_mock_units()) {
        for (auto & unit : datamock_unit.get_history_units()) {
            std::string header_bin;
            unit->get_header()->serialize_to_string(header_bin);
            std::string cert_bin;
            unit->get_cert()->serialize_to_string(cert_bin);
            std::string input_bin;
            unit->get_input()->serialize_to_string(input_bin);            
            std::string output_bin;
            unit->get_output()->serialize_to_string(output_bin);

            base::xstream_t stream(base::xcontext_t::instance());
            unit->full_block_serialize_to(stream);
            std::cout << "unit = " <<  unit->get_account() << " height=" << unit->get_height() << " size = " << stream.size()  
            << " header:" << header_bin.size()
            << " qcert:" << cert_bin.size()
            << " input:" << input_bin.size()
            << " output:" << output_bin.size()          
            << " input_res:" << unit->get_input()->get_resources_data().size() 
            << " output_res:" << unit->get_output()->get_resources_data().size()                        
            << " binlog:" << unit->get_binlog().size() << std::endl;// TODO(jimmy) size 590-660
        }
    }

    db::xdb_meta_t dbmeta = creator.get_xdb()->get_meta();
    std::cout << "db key size = " << dbmeta.m_db_key_size << std::endl;
    std::cout << "db value size = " << dbmeta.m_db_value_size << std::endl;
    std::cout << "hash_calc_count = " << xhashtest_t::hash_calc_count << std::endl;
    xhashtest_t::print_hash_calc = false;
}


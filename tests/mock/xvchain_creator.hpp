#pragma once
#include <string>
#include "gtest/gtest.h"
#include "xstore/xstore_face.h"
#include "xblockstore/xblockstore_face.h"
#include "xvledger/xvaccount.h"
#include "xvledger/xvblockstore.h"
#include "xvledger/xvledger.h"
#include "tests/mock/xtestdb.hpp"

namespace top
{
    namespace mock
    {
        class xvchain_creator {
        public:
            xvchain_creator() {
                base::xvchain_t::instance().clean_all(true);

                m_bus = top::make_object_ptr<mbus::xmessage_bus_t>(true, 1000);
                base::xvchain_t::instance().set_xevmbus(m_bus.get());

                m_store = store::xstore_factory::create_store_with_memdb();
                base::xvchain_t::instance().set_xdbstore(m_store.get());

                base::xvblockstore_t * blockstore = store::create_vblockstore(m_store.get());
                base::xvchain_t::instance().set_xblockstore(blockstore);
            }

            void create_blockstore_with_xstore() {
                // base::xvchain_t::instance().clean_all(true);
                // mock::xveventbus_impl* mbus_store = new mock::xveventbus_impl();
                // base::xvchain_t::instance().set_xevmbus(mbus_store);

                // m_store = store::xstore_factory::create_store_with_memdb();
                // base::xvchain_t::instance().set_xdbstore(m_store.get());

                // base::xvblockstore_t * blockstore = store::create_vblockstore(m_store.get());
                // base::xvchain_t::instance().set_xblockstore(blockstore);
            }
            base::xvblockstore_t* get_blockstore() const { return base::xvchain_t::instance().get_xblockstore(); }
            base::xvblkstatestore_t*    get_xblkstatestore() const { return base::xvchain_t::instance().get_xstatestore()->get_blkstate_store(); }
            void clean_all() {
                base::xvchain_t::instance().clean_all(false);
            }
            store::xstore_face_t* get_xstore() const {return m_store.get();}
            xobject_ptr_t<mbus::xmessage_bus_face_t> get_mbus() const {return m_bus;}
            
        private:
            xobject_ptr_t<store::xstore_face_t>  m_store{nullptr};
            xobject_ptr_t<mbus::xmessage_bus_face_t> m_bus;
        };
    }
}


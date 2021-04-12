// Copyright (c) 2018-2020 Telos Foundation & contributors
// taylor.wei@topnetwork.org
// Licensed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xbase/xcontext.h"
#include "xbase/xutl.h"
#include "../xvstate.h"

namespace top
{
    namespace base
    {
        //*************************************xvbstate_t****************************************
        void xvbstate_t::register_object(xcontext_t & context)
        {
            xstringvar_t::register_object(context);
            xtokenvar_t::register_object(context);
            xnoncevar_t::register_object(context);
            xcodevar_t::register_object(context);
            xmtokens_t::register_object(context);
            xmkeys_t::register_object(context);
            
            xhashmapvar_t::register_object(context);
            
            xvintvar_t<int64_t>::register_object(context);
            xvintvar_t<uint64_t>::register_object(context);
            
            xdequevar_t<int8_t>::register_object(context);
            xdequevar_t<int16_t>::register_object(context);
            xdequevar_t<int32_t>::register_object(context);
            xdequevar_t<int64_t>::register_object(context);
            xdequevar_t<uint64_t>::register_object(context);
            xdequevar_t<std::string>::register_object(context);
            
            xmapvar_t<int8_t>::register_object(context);
            xmapvar_t<int16_t>::register_object(context);
            xmapvar_t<int32_t>::register_object(context);
            xmapvar_t<int64_t>::register_object(context);
            xmapvar_t<uint64_t>::register_object(context);
            xmapvar_t<std::string>::register_object(context);
            //add others standard property
        }
    
        const std::string  xvbstate_t::make_unit_name(const std::string & account, const uint64_t blockheight)
        {
            const std::string compose_name = account + "." + xstring_utl::tostring(blockheight);
            return xstring_utl::tostring((uint32_t)xhash64_t::digest(compose_name));//to save space,let use hash32 as unit name for vbstate
        }

        xvbstate_t::xvbstate_t(enum_xdata_type type)
            :base(type)
        {
            //init unit name and block height first
            m_block_height = 0;
            //then set unit name
            set_unit_name(make_unit_name(std::string(),m_block_height));
            //ask compressed data while serialization
            set_unit_flag(enum_xdata_flag_acompress);
            
            xauto_ptr<xvcanvas_t> new_canvas(new xvcanvas_t());
            set_canvas(new_canvas.get());
            
            //then register execution methods
            REGISTER_XVIFUNC_ID_API(enum_xvinstruct_class_state_function);
        }
    
        xvbstate_t::xvbstate_t(const std::string & account_addr,const uint64_t block_height,const std::vector<xvproperty_t*> & properties,enum_xdata_type type)
            :base(type)
        {
            //init unit name and block height first
            m_account_addr = account_addr;
            m_block_height = block_height;
            //then set unit name
            set_unit_name(make_unit_name(m_account_addr,m_block_height));
            //ask compressed data while serialization
            set_unit_flag(enum_xdata_flag_acompress);

            xauto_ptr<xvcanvas_t> new_canvas(new xvcanvas_t());
            set_canvas(new_canvas.get());
            
            //then register execution methods
            REGISTER_XVIFUNC_ID_API(enum_xvinstruct_class_state_function);
            
            //finally add property object
            for(auto & p : properties)
            {
                if(p != nullptr)
                    add_child_unit(p);
            }
        }
    
        xvbstate_t::xvbstate_t(const xvbstate_t & obj)
            :base(obj)
        {
            m_account_addr = obj.m_account_addr;
            m_block_height = obj.m_block_height;
            m_block_output_hash = obj.m_block_output_hash;
            set_unit_name(make_unit_name(m_account_addr,m_block_height)); //set unit name first
            //ask compressed data while serialization
            set_unit_flag(enum_xdata_flag_acompress);
            
            //setup canvas
            xauto_ptr<xvcanvas_t> new_canvas(new xvcanvas_t());
            set_canvas(new_canvas.get());
            //then register execution methods
            REGISTER_XVIFUNC_ID_API(enum_xvinstruct_class_state_function);
            
            //finally set parent ptr
            set_parent_unit(obj.get_parent_unit());
        }
    
        xvbstate_t::xvbstate_t(const uint64_t new_block_height,const xvbstate_t & source)
            :base(source)
        {
            m_account_addr = source.m_account_addr;
            m_block_height = new_block_height;
            set_unit_name(make_unit_name(m_account_addr,m_block_height)); //set unit name first
            //ask compressed data while serialization
            set_unit_flag(enum_xdata_flag_acompress);
            
            //setup canvas
            xauto_ptr<xvcanvas_t> new_canvas(new xvcanvas_t());
            set_canvas(new_canvas.get());
            //then register execution methods
            REGISTER_XVIFUNC_ID_API(enum_xvinstruct_class_state_function);
            
            //finally set parent ptr
            set_parent_unit(source.get_parent_unit());
        }
        
        xvbstate_t::~xvbstate_t()
        {
        }
        
        xvexeunit_t* xvbstate_t::clone() //each property is readonly after clone
        {
            return new xvbstate_t(*this);
        }
    
        xvbstate_t* xvbstate_t::clone(const uint64_t clone_to_new_block_height) //each property is readonly after clone
        {
            return new xvbstate_t(clone_to_new_block_height,*this);
        }
        
        std::string xvbstate_t::dump() const
        {
            return std::string();
        }
 
        void*   xvbstate_t::query_interface(const int32_t _enum_xobject_type_)//caller need to cast (void*) to related ptr
        {
            if(_enum_xobject_type_ == enum_xobject_type_vbstate)
                return this;
            
            return base::query_interface(_enum_xobject_type_);
        }
    
        //clear canvas assocaited with vbstate and properties,all recored instruction are clear as well
        //note:reset_canvas not modify the actua state of properties/block, it just against for instrution on canvas
        bool         xvbstate_t::reset_canvas()
        {
            //setup canvas
            xauto_ptr<xvcanvas_t> new_canvas(new xvcanvas_t());
            set_canvas(new_canvas.get());
            return true;
        }

        std::string  xvbstate_t::get_property_value(const std::string & name)
        {
            std::string bin_data;
            xvproperty_t* target = get_property_object(name);
            if(target != nullptr)
                target->serialize_to_string(bin_data);

            return bin_data;
        }
        
        xvproperty_t*   xvbstate_t::get_property_object(const std::string & name)
        {
            xvexeunit_t * target = find_child_unit(name);
            if(target != nullptr)
                return (xvproperty_t*)target;
            
            return nullptr;
        }
    
        bool  xvbstate_t::find_property(const std::string & property_name) //check whether property already existing
        {
            if(get_property_object(property_name) != nullptr)
                return true;
            
            return false;
        }

        //subclass extend behavior and load more information instead of a raw one
        //return how many bytes readout /writed in, return < 0(enum_xerror_code_type) when have error
        int32_t    xvbstate_t::do_write(xstream_t & stream)  //allow subclass extend behavior
        {
            const int32_t begin_size = stream.size();
            
            stream.write_tiny_string(m_account_addr);
            stream.write_tiny_string(m_block_output_hash);
            stream << m_block_height;

            base::do_write(stream);
            return (stream.size() - begin_size);
        }
        
        int32_t   xvbstate_t::do_read(xstream_t & stream) //allow subclass extend behavior
        {
            const int32_t begin_size = stream.size();
            
            stream.read_tiny_string(m_account_addr);
            stream.read_tiny_string(m_block_output_hash);
            stream >> m_block_height;
            //set unit name immidiately after read them
            set_unit_name(make_unit_name(m_account_addr,m_block_height));
            
            base::do_read(stream);
            
            return (begin_size - stream.size());
        }
    
        xauto_ptr<xtokenvar_t>  xvbstate_t::new_token_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_token));
            if(result.get_error() == enum_xcode_successful)
                return load_token_var(property_name);
            
            xerror("xvbstate_t::new_token_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
    
        xauto_ptr<xnoncevar_t>      xvbstate_t::new_nonce_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_nonce));
            if(result.get_error() == enum_xcode_successful)
                return load_nonce_var(property_name);
            
            xerror("xvbstate_t::new_nonce_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
 
        xauto_ptr<xcodevar_t>   xvbstate_t::new_code_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_code));
            if(result.get_error() == enum_xcode_successful)
                return load_code_var(property_name);
            
            xerror("xvbstate_t::new_code_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
    
        xauto_ptr<xmtokens_t>   xvbstate_t::new_multiple_tokens_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_mtokens));
            if(result.get_error() == enum_xcode_successful)
                return load_multiple_tokens_var(property_name);
            
            xerror("xvbstate_t::new_multiple_tokens_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
    
        xauto_ptr<xmkeys_t>   xvbstate_t::new_multiple_keys_var(const std::string & property_name) //to manage pubkeys of account
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_mkeys));
            if(result.get_error() == enum_xcode_successful)
                return load_multiple_keys_var(property_name);
            
            xerror("xvbstate_t::new_multiple_keys_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
   
        xauto_ptr<xstringvar_t> xvbstate_t::new_string_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_string));
            if(result.get_error() == enum_xcode_successful)
                return load_string_var(property_name);
            
            xerror("xvbstate_t::new_string_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }

        xauto_ptr<xhashmapvar_t>   xvbstate_t::new_hashmap_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_hashmap));
            if(result.get_error() == enum_xcode_successful)
                return load_hashmap_var(property_name);
            
            xerror("xvbstate_t::new_hashmap_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }

        //integer related
        xauto_ptr<xvintvar_t<int64_t>> xvbstate_t::new_int64_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_int64));
            if(result.get_error() == enum_xcode_successful)
                return load_int64_var(property_name);
            
            xerror("xvbstate_t::new_int64_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xvintvar_t<uint64_t>> xvbstate_t::new_uint64_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_uint64));
            if(result.get_error() == enum_xcode_successful)
                return load_uint64_var(property_name);
            
            xerror("xvbstate_t::new_uint64_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
    
        //xdequevar_t related
        xauto_ptr<xdequevar_t<int8_t>>   xvbstate_t::new_int8_deque_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_int8_deque));
            if(result.get_error() == enum_xcode_successful)
                return load_int8_deque_var(property_name);
            
            xerror("xvbstate_t::new_int8_deque_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
     
        xauto_ptr<xdequevar_t<int16_t>>  xvbstate_t::new_int16_deque_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_int16_deque));
            if(result.get_error() == enum_xcode_successful)
                return load_int16_deque_var(property_name);
            
            xerror("xvbstate_t::new_int16_deque_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xdequevar_t<int32_t>>  xvbstate_t::new_int32_deque_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_int32_deque));
            if(result.get_error() == enum_xcode_successful)
                return load_int32_deque_var(property_name);
            
            xerror("xvbstate_t::new_int32_deque_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xdequevar_t<int64_t>>  xvbstate_t::new_int64_deque_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_int64_deque));
            if(result.get_error() == enum_xcode_successful)
                return load_int64_deque_var(property_name);
            
            xerror("xvbstate_t::new_int64_deque_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xdequevar_t<uint64_t>>  xvbstate_t::new_uint64_deque_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_uint64_deque));
            if(result.get_error() == enum_xcode_successful)
                return load_uint64_deque_var(property_name);
            
            xerror("xvbstate_t::new_uint64_deque_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xdequevar_t<std::string>>  xvbstate_t::new_string_deque_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_string_deque));
            if(result.get_error() == enum_xcode_successful)
                return load_string_deque_var(property_name);
            
            xerror("xvbstate_t::new_string_deque_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
    
        //xmapvar_t related
        xauto_ptr<xmapvar_t<int8_t>>   xvbstate_t::new_int8_map_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_int8_map));
            if(result.get_error() == enum_xcode_successful)
                return load_int8_map_var(property_name);
            
            xerror("xvbstate_t::new_int8_map_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xmapvar_t<int16_t>>   xvbstate_t::new_int16_map_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_int16_map));
            if(result.get_error() == enum_xcode_successful)
                return load_int16_map_var(property_name);
            
            xerror("xvbstate_t::new_int16_map_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xmapvar_t<int32_t>>   xvbstate_t::new_int32_map_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_int32_map));
            if(result.get_error() == enum_xcode_successful)
                return load_int32_map_var(property_name);
            
            xerror("xvbstate_t::new_int32_map_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xmapvar_t<int64_t>>   xvbstate_t::new_int64_map_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_int64_map));
            if(result.get_error() == enum_xcode_successful)
                return load_int64_map_var(property_name);
            
            xerror("xvbstate_t::new_int64_map_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
    
        xauto_ptr<xmapvar_t<uint64_t>>   xvbstate_t::new_uint64_map_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_uint64_map));
            if(result.get_error() == enum_xcode_successful)
                return load_uint64_map_var(property_name);
            
            xerror("xvbstate_t::new_uint64_map_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
    
        xauto_ptr<xmapvar_t<std::string>>   xvbstate_t::new_string_map_var(const std::string & property_name)
        {
            const xvalue_t result(new_property_internal(property_name,enum_xobject_type_vprop_string_map));
            if(result.get_error() == enum_xcode_successful)
                return load_string_map_var(property_name);
            
            xerror("xvbstate_t::new_string_map_var,failed as error:%d for name(%s)",result.get_error(),property_name.c_str());
            return nullptr;
        }
    
        xauto_ptr<xtokenvar_t>  xvbstate_t::load_token_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xtokenvar_t* token_obj = (xtokenvar_t*)property_obj->query_interface(enum_xobject_type_vprop_token);
                xassert(token_obj != nullptr);
                if(token_obj != nullptr)
                    token_obj->add_ref();//for returned xauto_ptr
                return token_obj;
            }
            xerror("xvbstate_t::load_token_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xnoncevar_t>  xvbstate_t::load_nonce_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xnoncevar_t* nonce_obj = (xnoncevar_t*)property_obj->query_interface(top::base::enum_xobject_type_vprop_nonce);
                xassert(nonce_obj != nullptr);
                if(nonce_obj != nullptr)
                    nonce_obj->add_ref();//for returned xauto_ptr
                return nonce_obj;
            }
            xerror("xvbstate_t::load_nonce_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
    
        xauto_ptr<xcodevar_t>  xvbstate_t::load_code_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xcodevar_t* code_obj = (xcodevar_t*)property_obj->query_interface(top::base::enum_xobject_type_vprop_code);
                xassert(code_obj != nullptr);
                if(code_obj != nullptr)
                    code_obj->add_ref();//for returned xauto_ptr
                return code_obj;
            }
            xerror("xvbstate_t::load_code_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
    
        xauto_ptr<xmtokens_t>    xvbstate_t::load_multiple_tokens_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xmtokens_t* tokens_obj = (xmtokens_t*)property_obj->query_interface(top::base::enum_xobject_type_vprop_mtokens);
                xassert(tokens_obj != nullptr);
                if(tokens_obj != nullptr)
                    tokens_obj->add_ref();//for returned xauto_ptr
                return tokens_obj;
            }
            xerror("xvbstate_t::load_multiple_tokens_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xmkeys_t>    xvbstate_t::load_multiple_keys_var(const std::string & property_name)//to manage pubkeys of account
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xmkeys_t* keys_obj = (xmkeys_t*)property_obj->query_interface(top::base::enum_xobject_type_vprop_mkeys);
                xassert(keys_obj != nullptr);
                if(keys_obj != nullptr)
                    keys_obj->add_ref();//for returned xauto_ptr
                return keys_obj;
            }
            xerror("xvbstate_t::load_multiple_keys_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
 
        xauto_ptr<xstringvar_t> xvbstate_t::load_string_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xstringvar_t* var_obj = (xstringvar_t*)property_obj->query_interface(top::base::enum_xobject_type_vprop_string);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_string_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }

        xauto_ptr<xhashmapvar_t>xvbstate_t::load_hashmap_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xhashmapvar_t* var_obj = (xhashmapvar_t*)property_obj->query_interface(top::base::enum_xobject_type_vprop_hashmap);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_hashmap_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        //integer related load functions
        xauto_ptr<xvintvar_t<int64_t>>  xvbstate_t::load_int64_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xvintvar_t<int64_t>* token_obj = (xvintvar_t<int64_t>*)property_obj->query_interface(top::base::enum_xobject_type_vprop_int64);
                xassert(token_obj != nullptr);
                if(token_obj != nullptr)
                    token_obj->add_ref();//for returned xauto_ptr
                return token_obj;
            }
            xerror("xvbstate_t::load_int64_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xvintvar_t<uint64_t>>  xvbstate_t::load_uint64_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xvintvar_t<uint64_t>* token_obj = (xvintvar_t<uint64_t>*)property_obj->query_interface(top::base::enum_xobject_type_vprop_uint64);
                xassert(token_obj != nullptr);
                if(token_obj != nullptr)
                    token_obj->add_ref();//for returned xauto_ptr
                return token_obj;
            }
            xerror("xvbstate_t::load_uint64_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        //load functions of deque related
        xauto_ptr<xdequevar_t<int8_t>> xvbstate_t::load_int8_deque_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xdequevar_t<int8_t>* var_obj = (xdequevar_t<int8_t>*)property_obj->query_interface(enum_xobject_type_vprop_int8_deque);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_int8_deque_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xdequevar_t<int16_t>> xvbstate_t::load_int16_deque_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xdequevar_t<int16_t>* var_obj = (xdequevar_t<int16_t>*)property_obj->query_interface(enum_xobject_type_vprop_int16_deque);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_int16_deque_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xdequevar_t<int32_t>> xvbstate_t::load_int32_deque_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xdequevar_t<int32_t>* var_obj = (xdequevar_t<int32_t>*)property_obj->query_interface(enum_xobject_type_vprop_int32_deque);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_int32_deque_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xdequevar_t<int64_t>> xvbstate_t::load_int64_deque_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xdequevar_t<int64_t>* var_obj = (xdequevar_t<int64_t>*)property_obj->query_interface(enum_xobject_type_vprop_int64_deque);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_int64_deque_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xdequevar_t<uint64_t>> xvbstate_t::load_uint64_deque_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xdequevar_t<uint64_t>* var_obj = (xdequevar_t<uint64_t>*)property_obj->query_interface(enum_xobject_type_vprop_uint64_deque);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_uint64_deque_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xdequevar_t<std::string>> xvbstate_t::load_string_deque_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xdequevar_t<std::string>* var_obj = (xdequevar_t<std::string>*)property_obj->query_interface(enum_xobject_type_vprop_string_deque);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_string_deque_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
    
        //load functions of map related
        xauto_ptr<xmapvar_t<int8_t>>   xvbstate_t::load_int8_map_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xmapvar_t<int8_t>* var_obj = (xmapvar_t<int8_t>*)property_obj->query_interface(enum_xobject_type_vprop_int8_map);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_int8_map_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xmapvar_t<int16_t>>   xvbstate_t::load_int16_map_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xmapvar_t<int16_t>* var_obj = (xmapvar_t<int16_t>*)property_obj->query_interface(enum_xobject_type_vprop_int16_map);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_int16_map_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xmapvar_t<int32_t>>   xvbstate_t::load_int32_map_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xmapvar_t<int32_t>* var_obj = (xmapvar_t<int32_t>*)property_obj->query_interface(enum_xobject_type_vprop_int32_map);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_int32_map_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xmapvar_t<int64_t>>   xvbstate_t::load_int64_map_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xmapvar_t<int64_t>* var_obj = (xmapvar_t<int64_t>*)property_obj->query_interface(enum_xobject_type_vprop_int64_map);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_int64_map_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xmapvar_t<uint64_t>>   xvbstate_t::load_uint64_map_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xmapvar_t<uint64_t>* var_obj = (xmapvar_t<uint64_t>*)property_obj->query_interface(enum_xobject_type_vprop_uint64_map);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_uint64_map_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
        
        xauto_ptr<xmapvar_t<std::string>>   xvbstate_t::load_string_map_var(const std::string & property_name)
        {
            xvproperty_t * property_obj = get_property_object(property_name);
            if(property_obj != nullptr)
            {
                xmapvar_t<std::string>* var_obj = (xmapvar_t<std::string>*)property_obj->query_interface(enum_xobject_type_vprop_string_map);
                xassert(var_obj != nullptr);
                if(var_obj != nullptr)
                    var_obj->add_ref();//for returned xauto_ptr
                return var_obj;
            }
            xerror("xvbstate_t::load_string_map_var,failed to load for name(%s)",property_name.c_str());
            return nullptr;
        }
    
        xauto_ptr<xvproperty_t> xvbstate_t::load_property(const std::string & property_name)
        {
            xvproperty_t * found_target = get_property_object(property_name);
            if(found_target != nullptr)
                found_target->add_ref();//add reference for xauto_ptr;
            return found_target;
        }
    
        xauto_ptr<xvproperty_t> xvbstate_t::new_property(const std::string & property_name,const int propertyType)
        {
            const xvalue_t result(new_property_internal(property_name,propertyType));
            if(result.get_error() == enum_xcode_successful)
                return load_property(property_name);
            
            xerror("xvbstate_t::new_property,failed as error:%d for name(%s) and type(%d)",result.get_error(),property_name.c_str(),propertyType);
            return nullptr;
        }
    
        const xvalue_t xvbstate_t::new_property_internal(const std::string & property_name,const int propertyType)
        {
            xvalue_t param_ptype((vint32_t)propertyType);
            xvalue_t param_pname(property_name);
            xvmethod_t instruction(get_execute_uri(),enum_xvinstruct_class_state_function,enum_xvinstruct_state_method_new_property ,param_ptype,param_pname);
            
            return execute(instruction,(xvcanvas_t*)get_canvas()); //excute the instruction
        }
    
        const xvalue_t  xvbstate_t::do_new_property(const xvmethod_t & op)
        {
            const xvalue_t & propertyType = op.get_method_params().at(0);
            const xvalue_t & property_name = op.get_method_params().at(1);
            
            xvproperty_t* existingOne = get_property_object(property_name.get_string());
            if(existingOne != nullptr)
            {
                xerror("xvbstate_t::do_new_property,try overwrite existing property of account(%s) at height(%lld),new-property(%s)",get_account_addr().c_str(),get_block_height(),property_name.get_string().c_str());
                return xvalue_t(enum_xerror_code_exist);
            }
            if(get_childs_count() >= enum_max_property_count)
            {
                xerror("xvbstate_t::do_new_property,the account has too many properites already,account(%s) at height(%lld)",get_account_addr().c_str(),get_block_height());
                return xvalue_t(enum_xerror_code_over_limit);
            }
            
            xauto_ptr<xobject_t> object(xcontext_t::create_xobject((enum_xobject_type)propertyType.get_int32()));
            xassert(object);
            if(object)
            {
                xvproperty_t * property_obj = (xvproperty_t*)object->query_interface(enum_xobject_type_vproperty);
                xassert(property_obj != NULL);
                if(property_obj != NULL)
                {
                    property_obj->set_unit_name(property_name.get_string());
                    property_obj->set_canvas(get_canvas());
                    add_child_unit(property_obj);
                    
                    return xvalue_t(enum_xcode_successful);
                }
                else
                {
                    return xvalue_t(enum_xerror_code_bad_object);
                }
            }
            else
            {
                return xvalue_t(enum_xerror_code_not_found);
            }
        }
        
        //---------------------------------bin log ---------------------------------//
        enum_xerror_code  xvbstate_t::encode_change_to_binlog(std::string & output_bin)
        {
            try{
                const int result = get_canvas()->encode(xvcanvas_t::enum_compile_optimization_all,output_bin);
                if(result >= enum_xcode_successful)
                {
                    const char bin_type = '1';  //version#1: new bin-log format
                    output_bin.append(1,bin_type);
                    return enum_xcode_successful;
                }
                else
                {
                    xerror("xvbstate_t::encode_change_to_binlog,encode failed as error:%d",result);
                    return (enum_xerror_code)result;
                }
            } catch (int error_code){
                xerror("xvbstate_t::encode_change_to_binlog,throw exception with error:%d",error_code);
                return enum_xerror_code_errno;
            }
            xerror("xvbstate_t::encode_change_to_binlog,throw unknow exception");
            return enum_xerror_code_fail;
        }
    
        enum_xerror_code  xvbstate_t::encode_change_to_binlog(xstream_t & _ouptput_stream)
        {
            try{
                const int result = get_canvas()->encode(xvcanvas_t::enum_compile_optimization_all,_ouptput_stream);
                //const int result = get_canvas()->encode(xvcanvas_t::enum_compile_optimization_none,_ouptput_stream);
                if(result >= enum_xcode_successful)
                {
                    const char bin_type = '1';  //version#1: new bin-log format
                    _ouptput_stream.push_back((uint8_t*)&bin_type, 1);
                    return enum_xcode_successful;
                }
                else
                {
                    xerror("xvbstate_t::encode_change_to_binlog,encode failed as error:%d",result);
                    return (enum_xerror_code)result;
                }
            } catch (int error_code){
                xerror("xvbstate_t::encode_change_to_binlog,throw exception with error:%d",error_code);
                return enum_xerror_code_errno;
            }
            xerror("xvbstate_t::encode_change_to_binlog,throw unknow exception");
            return enum_xerror_code_fail;
        }
        
        enum_xerror_code   xvbstate_t::decode_change_from_binlog(const std::string & from_bin_log,std::deque<top::base::xvmethod_t> & out_records)
        {
            xstream_t _input_stream(xcontext_t::instance(),(uint8_t*)from_bin_log.data(),(uint32_t)from_bin_log.size());
            return decode_change_from_binlog(_input_stream,_input_stream.size(),out_records);
        }
    
        enum_xerror_code   xvbstate_t::decode_change_from_binlog(xstream_t & from_bin_log,const uint32_t bin_log_size,std::deque<top::base::xvmethod_t> & out_records)
        {
            if( (from_bin_log.size() < 1) || ((uint32_t)from_bin_log.size() < bin_log_size) )//at least one bytes
            {
                xerror("xvbstate_t::decode_from_binlog,invalid stream of size(%d) vs bin_log_size(%u)",(int)from_bin_log.size(),bin_log_size);
                return enum_xerror_code_no_data;
            }

            try{
                const char bin_type = *((char*)from_bin_log.data() + bin_log_size - 1);
                if(bin_type >= '1')
                {
                    const int result = xvcanvas_t::decode(from_bin_log,bin_log_size - 1,out_records);
                    if(result >= enum_xcode_successful)
                        from_bin_log.pop_front(1);//pop bin_type now
                    else
                        xerror("xvbstate_t::decode_from_binlog,failed as error(%d)",result);
                    
                    return (enum_xerror_code)result;
                }
                else
                {
                    xerror("xvbstate_t::decode_from_binlog,invalid bin_type(%c)",bin_type);
                    return enum_xerror_code_bad_type;
                }
                
            } catch (int error_code){
                xerror("xvbstate_t::decode_from_binlog,throw exception with error:%d",error_code);
                return enum_xerror_code_errno;
            }
            xerror("xvbstate_t::decode_from_binlog,throw unknow exception");
            return enum_xerror_code_fail;
        }
        
        bool   xvbstate_t::apply_changes_of_binlog(const std::string & from_bin_log) //apply changes to current states
        {
            std::deque<top::base::xvmethod_t> out_records;
            if(decode_change_from_binlog(from_bin_log, out_records) >= enum_xcode_successful)
            {
                for(auto & op : out_records)
                {
                    execute(op,nullptr);
                }
                return true;
            }
            #ifdef DEBUG
                xerror("decompile_from_binlog failed for bin-log(%s)",from_bin_log.c_str());
            #else
                xerror("decompile_from_binlog failed for bin-log,length(%u)",(uint32_t)from_bin_log.size());
            #endif
            return false;
        }
    
        bool   xvbstate_t::apply_changes_of_binlog(xstream_t & from_bin_log,const uint32_t bin_log_size) //apply changes to current states
        {
            std::deque<top::base::xvmethod_t> out_records;
            if(decode_change_from_binlog(from_bin_log,bin_log_size,out_records) >= enum_xcode_successful)
            {
                for(auto & op : out_records)
                {
                    execute(op,nullptr);
                }
                return true;
            }
            xerror("decompile_from_binlog failed for bin-log,length(%u)",(uint32_t)from_bin_log.size());
            return false;
        }
    };
};
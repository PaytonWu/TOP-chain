// This file is generated by rust-protobuf 2.27.1. Do not edit
// @generated

// https://github.com/rust-lang/rust-clippy/issues/702
#![allow(unknown_lints)]
#![allow(clippy::all)]

#![allow(unused_attributes)]
#![cfg_attr(rustfmt, rustfmt::skip)]

#![allow(box_pointers)]
#![allow(dead_code)]
#![allow(missing_docs)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(non_upper_case_globals)]
#![allow(trivial_casts)]
#![allow(unused_imports)]
#![allow(unused_results)]
//! Generated file from `proto_basic.proto`

/// Generated files are compatible only with the same version
/// of protobuf runtime.
// const _PROTOBUF_VERSION_CHECK: () = ::protobuf::VERSION_2_27_1;

#[derive(PartialEq,Clone,Default)]
pub struct ProtoAddress {
    // message fields
    pub value: ::std::vec::Vec<u8>,
    // special fields
    pub unknown_fields: ::protobuf::UnknownFields,
    pub cached_size: ::protobuf::CachedSize,
}

impl<'a> ::std::default::Default for &'a ProtoAddress {
    fn default() -> &'a ProtoAddress {
        <ProtoAddress as ::protobuf::Message>::default_instance()
    }
}

impl ProtoAddress {
    pub fn new() -> ProtoAddress {
        ::std::default::Default::default()
    }

    // bytes value = 1;


    pub fn get_value(&self) -> &[u8] {
        &self.value
    }
    pub fn clear_value(&mut self) {
        self.value.clear();
    }

    // Param is passed by value, moved
    pub fn set_value(&mut self, v: ::std::vec::Vec<u8>) {
        self.value = v;
    }

    // Mutable pointer to the field.
    // If field is not initialized, it is initialized with default value first.
    pub fn mut_value(&mut self) -> &mut ::std::vec::Vec<u8> {
        &mut self.value
    }

    // Take field
    pub fn take_value(&mut self) -> ::std::vec::Vec<u8> {
        ::std::mem::replace(&mut self.value, ::std::vec::Vec::new())
    }
}

impl ::protobuf::Message for ProtoAddress {
    fn is_initialized(&self) -> bool {
        true
    }

    fn merge_from(&mut self, is: &mut ::protobuf::CodedInputStream<'_>) -> ::protobuf::ProtobufResult<()> {
        while !is.eof()? {
            let (field_number, wire_type) = is.read_tag_unpack()?;
            match field_number {
                1 => {
                    ::protobuf::rt::read_singular_proto3_bytes_into(wire_type, is, &mut self.value)?;
                },
                _ => {
                    ::protobuf::rt::read_unknown_or_skip_group(field_number, wire_type, is, self.mut_unknown_fields())?;
                },
            };
        }
        ::std::result::Result::Ok(())
    }

    // Compute sizes of nested messages
    #[allow(unused_variables)]
    fn compute_size(&self) -> u32 {
        let mut my_size = 0;
        if !self.value.is_empty() {
            my_size += ::protobuf::rt::bytes_size(1, &self.value);
        }
        my_size += ::protobuf::rt::unknown_fields_size(self.get_unknown_fields());
        self.cached_size.set(my_size);
        my_size
    }

    fn write_to_with_cached_sizes(&self, os: &mut ::protobuf::CodedOutputStream<'_>) -> ::protobuf::ProtobufResult<()> {
        if !self.value.is_empty() {
            os.write_bytes(1, &self.value)?;
        }
        os.write_unknown_fields(self.get_unknown_fields())?;
        ::std::result::Result::Ok(())
    }

    fn get_cached_size(&self) -> u32 {
        self.cached_size.get()
    }

    fn get_unknown_fields(&self) -> &::protobuf::UnknownFields {
        &self.unknown_fields
    }

    fn mut_unknown_fields(&mut self) -> &mut ::protobuf::UnknownFields {
        &mut self.unknown_fields
    }

    fn as_any(&self) -> &dyn (::std::any::Any) {
        self as &dyn (::std::any::Any)
    }
    fn as_any_mut(&mut self) -> &mut dyn (::std::any::Any) {
        self as &mut dyn (::std::any::Any)
    }
    fn into_any(self: ::std::boxed::Box<Self>) -> ::std::boxed::Box<dyn (::std::any::Any)> {
        self
    }

    fn descriptor(&self) -> &'static ::protobuf::reflect::MessageDescriptor {
        Self::descriptor_static()
    }

    fn new() -> ProtoAddress {
        ProtoAddress::new()
    }

    fn descriptor_static() -> &'static ::protobuf::reflect::MessageDescriptor {
        static descriptor: ::protobuf::rt::LazyV2<::protobuf::reflect::MessageDescriptor> = ::protobuf::rt::LazyV2::INIT;
        descriptor.get(|| {
            let mut fields = ::std::vec::Vec::new();
            fields.push(::protobuf::reflect::accessor::make_simple_field_accessor::<_, ::protobuf::types::ProtobufTypeBytes>(
                "value",
                |m: &ProtoAddress| { &m.value },
                |m: &mut ProtoAddress| { &mut m.value },
            ));
            ::protobuf::reflect::MessageDescriptor::new_pb_name::<ProtoAddress>(
                "ProtoAddress",
                fields,
                file_descriptor_proto()
            )
        })
    }

    fn default_instance() -> &'static ProtoAddress {
        static instance: ::protobuf::rt::LazyV2<ProtoAddress> = ::protobuf::rt::LazyV2::INIT;
        instance.get(ProtoAddress::new)
    }
}

impl ::protobuf::Clear for ProtoAddress {
    fn clear(&mut self) {
        self.value.clear();
        self.unknown_fields.clear();
    }
}

impl ::std::fmt::Debug for ProtoAddress {
    fn fmt(&self, f: &mut ::std::fmt::Formatter<'_>) -> ::std::fmt::Result {
        ::protobuf::text_format::fmt(self, f)
    }
}

impl ::protobuf::reflect::ProtobufValue for ProtoAddress {
    fn as_ref(&self) -> ::protobuf::reflect::ReflectValueRef {
        ::protobuf::reflect::ReflectValueRef::Message(self)
    }
}

#[derive(PartialEq,Clone,Default)]
pub struct RawU256 {
    // message fields
    pub data: ::std::vec::Vec<u8>,
    // special fields
    pub unknown_fields: ::protobuf::UnknownFields,
    pub cached_size: ::protobuf::CachedSize,
}

impl<'a> ::std::default::Default for &'a RawU256 {
    fn default() -> &'a RawU256 {
        <RawU256 as ::protobuf::Message>::default_instance()
    }
}

impl RawU256 {
    pub fn new() -> RawU256 {
        ::std::default::Default::default()
    }

    // bytes data = 1;


    pub fn get_data(&self) -> &[u8] {
        &self.data
    }
    pub fn clear_data(&mut self) {
        self.data.clear();
    }

    // Param is passed by value, moved
    pub fn set_data(&mut self, v: ::std::vec::Vec<u8>) {
        self.data = v;
    }

    // Mutable pointer to the field.
    // If field is not initialized, it is initialized with default value first.
    pub fn mut_data(&mut self) -> &mut ::std::vec::Vec<u8> {
        &mut self.data
    }

    // Take field
    pub fn take_data(&mut self) -> ::std::vec::Vec<u8> {
        ::std::mem::replace(&mut self.data, ::std::vec::Vec::new())
    }
}

impl ::protobuf::Message for RawU256 {
    fn is_initialized(&self) -> bool {
        true
    }

    fn merge_from(&mut self, is: &mut ::protobuf::CodedInputStream<'_>) -> ::protobuf::ProtobufResult<()> {
        while !is.eof()? {
            let (field_number, wire_type) = is.read_tag_unpack()?;
            match field_number {
                1 => {
                    ::protobuf::rt::read_singular_proto3_bytes_into(wire_type, is, &mut self.data)?;
                },
                _ => {
                    ::protobuf::rt::read_unknown_or_skip_group(field_number, wire_type, is, self.mut_unknown_fields())?;
                },
            };
        }
        ::std::result::Result::Ok(())
    }

    // Compute sizes of nested messages
    #[allow(unused_variables)]
    fn compute_size(&self) -> u32 {
        let mut my_size = 0;
        if !self.data.is_empty() {
            my_size += ::protobuf::rt::bytes_size(1, &self.data);
        }
        my_size += ::protobuf::rt::unknown_fields_size(self.get_unknown_fields());
        self.cached_size.set(my_size);
        my_size
    }

    fn write_to_with_cached_sizes(&self, os: &mut ::protobuf::CodedOutputStream<'_>) -> ::protobuf::ProtobufResult<()> {
        if !self.data.is_empty() {
            os.write_bytes(1, &self.data)?;
        }
        os.write_unknown_fields(self.get_unknown_fields())?;
        ::std::result::Result::Ok(())
    }

    fn get_cached_size(&self) -> u32 {
        self.cached_size.get()
    }

    fn get_unknown_fields(&self) -> &::protobuf::UnknownFields {
        &self.unknown_fields
    }

    fn mut_unknown_fields(&mut self) -> &mut ::protobuf::UnknownFields {
        &mut self.unknown_fields
    }

    fn as_any(&self) -> &dyn (::std::any::Any) {
        self as &dyn (::std::any::Any)
    }
    fn as_any_mut(&mut self) -> &mut dyn (::std::any::Any) {
        self as &mut dyn (::std::any::Any)
    }
    fn into_any(self: ::std::boxed::Box<Self>) -> ::std::boxed::Box<dyn (::std::any::Any)> {
        self
    }

    fn descriptor(&self) -> &'static ::protobuf::reflect::MessageDescriptor {
        Self::descriptor_static()
    }

    fn new() -> RawU256 {
        RawU256::new()
    }

    fn descriptor_static() -> &'static ::protobuf::reflect::MessageDescriptor {
        static descriptor: ::protobuf::rt::LazyV2<::protobuf::reflect::MessageDescriptor> = ::protobuf::rt::LazyV2::INIT;
        descriptor.get(|| {
            let mut fields = ::std::vec::Vec::new();
            fields.push(::protobuf::reflect::accessor::make_simple_field_accessor::<_, ::protobuf::types::ProtobufTypeBytes>(
                "data",
                |m: &RawU256| { &m.data },
                |m: &mut RawU256| { &mut m.data },
            ));
            ::protobuf::reflect::MessageDescriptor::new_pb_name::<RawU256>(
                "RawU256",
                fields,
                file_descriptor_proto()
            )
        })
    }

    fn default_instance() -> &'static RawU256 {
        static instance: ::protobuf::rt::LazyV2<RawU256> = ::protobuf::rt::LazyV2::INIT;
        instance.get(RawU256::new)
    }
}

impl ::protobuf::Clear for RawU256 {
    fn clear(&mut self) {
        self.data.clear();
        self.unknown_fields.clear();
    }
}

impl ::std::fmt::Debug for RawU256 {
    fn fmt(&self, f: &mut ::std::fmt::Formatter<'_>) -> ::std::fmt::Result {
        ::protobuf::text_format::fmt(self, f)
    }
}

impl ::protobuf::reflect::ProtobufValue for RawU256 {
    fn as_ref(&self) -> ::protobuf::reflect::ReflectValueRef {
        ::protobuf::reflect::ReflectValueRef::Message(self)
    }
}

#[derive(PartialEq,Clone,Default)]
pub struct WeiU256 {
    // message fields
    pub data: ::std::vec::Vec<u8>,
    // special fields
    pub unknown_fields: ::protobuf::UnknownFields,
    pub cached_size: ::protobuf::CachedSize,
}

impl<'a> ::std::default::Default for &'a WeiU256 {
    fn default() -> &'a WeiU256 {
        <WeiU256 as ::protobuf::Message>::default_instance()
    }
}

impl WeiU256 {
    pub fn new() -> WeiU256 {
        ::std::default::Default::default()
    }

    // bytes data = 1;


    pub fn get_data(&self) -> &[u8] {
        &self.data
    }
    pub fn clear_data(&mut self) {
        self.data.clear();
    }

    // Param is passed by value, moved
    pub fn set_data(&mut self, v: ::std::vec::Vec<u8>) {
        self.data = v;
    }

    // Mutable pointer to the field.
    // If field is not initialized, it is initialized with default value first.
    pub fn mut_data(&mut self) -> &mut ::std::vec::Vec<u8> {
        &mut self.data
    }

    // Take field
    pub fn take_data(&mut self) -> ::std::vec::Vec<u8> {
        ::std::mem::replace(&mut self.data, ::std::vec::Vec::new())
    }
}

impl ::protobuf::Message for WeiU256 {
    fn is_initialized(&self) -> bool {
        true
    }

    fn merge_from(&mut self, is: &mut ::protobuf::CodedInputStream<'_>) -> ::protobuf::ProtobufResult<()> {
        while !is.eof()? {
            let (field_number, wire_type) = is.read_tag_unpack()?;
            match field_number {
                1 => {
                    ::protobuf::rt::read_singular_proto3_bytes_into(wire_type, is, &mut self.data)?;
                },
                _ => {
                    ::protobuf::rt::read_unknown_or_skip_group(field_number, wire_type, is, self.mut_unknown_fields())?;
                },
            };
        }
        ::std::result::Result::Ok(())
    }

    // Compute sizes of nested messages
    #[allow(unused_variables)]
    fn compute_size(&self) -> u32 {
        let mut my_size = 0;
        if !self.data.is_empty() {
            my_size += ::protobuf::rt::bytes_size(1, &self.data);
        }
        my_size += ::protobuf::rt::unknown_fields_size(self.get_unknown_fields());
        self.cached_size.set(my_size);
        my_size
    }

    fn write_to_with_cached_sizes(&self, os: &mut ::protobuf::CodedOutputStream<'_>) -> ::protobuf::ProtobufResult<()> {
        if !self.data.is_empty() {
            os.write_bytes(1, &self.data)?;
        }
        os.write_unknown_fields(self.get_unknown_fields())?;
        ::std::result::Result::Ok(())
    }

    fn get_cached_size(&self) -> u32 {
        self.cached_size.get()
    }

    fn get_unknown_fields(&self) -> &::protobuf::UnknownFields {
        &self.unknown_fields
    }

    fn mut_unknown_fields(&mut self) -> &mut ::protobuf::UnknownFields {
        &mut self.unknown_fields
    }

    fn as_any(&self) -> &dyn (::std::any::Any) {
        self as &dyn (::std::any::Any)
    }
    fn as_any_mut(&mut self) -> &mut dyn (::std::any::Any) {
        self as &mut dyn (::std::any::Any)
    }
    fn into_any(self: ::std::boxed::Box<Self>) -> ::std::boxed::Box<dyn (::std::any::Any)> {
        self
    }

    fn descriptor(&self) -> &'static ::protobuf::reflect::MessageDescriptor {
        Self::descriptor_static()
    }

    fn new() -> WeiU256 {
        WeiU256::new()
    }

    fn descriptor_static() -> &'static ::protobuf::reflect::MessageDescriptor {
        static descriptor: ::protobuf::rt::LazyV2<::protobuf::reflect::MessageDescriptor> = ::protobuf::rt::LazyV2::INIT;
        descriptor.get(|| {
            let mut fields = ::std::vec::Vec::new();
            fields.push(::protobuf::reflect::accessor::make_simple_field_accessor::<_, ::protobuf::types::ProtobufTypeBytes>(
                "data",
                |m: &WeiU256| { &m.data },
                |m: &mut WeiU256| { &mut m.data },
            ));
            ::protobuf::reflect::MessageDescriptor::new_pb_name::<WeiU256>(
                "WeiU256",
                fields,
                file_descriptor_proto()
            )
        })
    }

    fn default_instance() -> &'static WeiU256 {
        static instance: ::protobuf::rt::LazyV2<WeiU256> = ::protobuf::rt::LazyV2::INIT;
        instance.get(WeiU256::new)
    }
}

impl ::protobuf::Clear for WeiU256 {
    fn clear(&mut self) {
        self.data.clear();
        self.unknown_fields.clear();
    }
}

impl ::std::fmt::Debug for WeiU256 {
    fn fmt(&self, f: &mut ::std::fmt::Formatter<'_>) -> ::std::fmt::Result {
        ::protobuf::text_format::fmt(self, f)
    }
}

impl ::protobuf::reflect::ProtobufValue for WeiU256 {
    fn as_ref(&self) -> ::protobuf::reflect::ReflectValueRef {
        ::protobuf::reflect::ReflectValueRef::Message(self)
    }
}

#[derive(PartialEq,Clone,Default)]
pub struct ResultLog {
    // message fields
    pub address: ::protobuf::SingularPtrField<ProtoAddress>,
    pub topics: ::protobuf::RepeatedField<RawU256>,
    pub data: ::std::vec::Vec<u8>,
    // special fields
    pub unknown_fields: ::protobuf::UnknownFields,
    pub cached_size: ::protobuf::CachedSize,
}

impl<'a> ::std::default::Default for &'a ResultLog {
    fn default() -> &'a ResultLog {
        <ResultLog as ::protobuf::Message>::default_instance()
    }
}

impl ResultLog {
    pub fn new() -> ResultLog {
        ::std::default::Default::default()
    }

    // .top.evm_engine.basic.ProtoAddress address = 1;


    pub fn get_address(&self) -> &ProtoAddress {
        self.address.as_ref().unwrap_or_else(|| <ProtoAddress as ::protobuf::Message>::default_instance())
    }
    pub fn clear_address(&mut self) {
        self.address.clear();
    }

    pub fn has_address(&self) -> bool {
        self.address.is_some()
    }

    // Param is passed by value, moved
    pub fn set_address(&mut self, v: ProtoAddress) {
        self.address = ::protobuf::SingularPtrField::some(v);
    }

    // Mutable pointer to the field.
    // If field is not initialized, it is initialized with default value first.
    pub fn mut_address(&mut self) -> &mut ProtoAddress {
        if self.address.is_none() {
            self.address.set_default();
        }
        self.address.as_mut().unwrap()
    }

    // Take field
    pub fn take_address(&mut self) -> ProtoAddress {
        self.address.take().unwrap_or_else(|| ProtoAddress::new())
    }

    // repeated .top.evm_engine.basic.RawU256 topics = 2;


    pub fn get_topics(&self) -> &[RawU256] {
        &self.topics
    }
    pub fn clear_topics(&mut self) {
        self.topics.clear();
    }

    // Param is passed by value, moved
    pub fn set_topics(&mut self, v: ::protobuf::RepeatedField<RawU256>) {
        self.topics = v;
    }

    // Mutable pointer to the field.
    pub fn mut_topics(&mut self) -> &mut ::protobuf::RepeatedField<RawU256> {
        &mut self.topics
    }

    // Take field
    pub fn take_topics(&mut self) -> ::protobuf::RepeatedField<RawU256> {
        ::std::mem::replace(&mut self.topics, ::protobuf::RepeatedField::new())
    }

    // bytes data = 3;


    pub fn get_data(&self) -> &[u8] {
        &self.data
    }
    pub fn clear_data(&mut self) {
        self.data.clear();
    }

    // Param is passed by value, moved
    pub fn set_data(&mut self, v: ::std::vec::Vec<u8>) {
        self.data = v;
    }

    // Mutable pointer to the field.
    // If field is not initialized, it is initialized with default value first.
    pub fn mut_data(&mut self) -> &mut ::std::vec::Vec<u8> {
        &mut self.data
    }

    // Take field
    pub fn take_data(&mut self) -> ::std::vec::Vec<u8> {
        ::std::mem::replace(&mut self.data, ::std::vec::Vec::new())
    }
}

impl ::protobuf::Message for ResultLog {
    fn is_initialized(&self) -> bool {
        for v in &self.address {
            if !v.is_initialized() {
                return false;
            }
        };
        for v in &self.topics {
            if !v.is_initialized() {
                return false;
            }
        };
        true
    }

    fn merge_from(&mut self, is: &mut ::protobuf::CodedInputStream<'_>) -> ::protobuf::ProtobufResult<()> {
        while !is.eof()? {
            let (field_number, wire_type) = is.read_tag_unpack()?;
            match field_number {
                1 => {
                    ::protobuf::rt::read_singular_message_into(wire_type, is, &mut self.address)?;
                },
                2 => {
                    ::protobuf::rt::read_repeated_message_into(wire_type, is, &mut self.topics)?;
                },
                3 => {
                    ::protobuf::rt::read_singular_proto3_bytes_into(wire_type, is, &mut self.data)?;
                },
                _ => {
                    ::protobuf::rt::read_unknown_or_skip_group(field_number, wire_type, is, self.mut_unknown_fields())?;
                },
            };
        }
        ::std::result::Result::Ok(())
    }

    // Compute sizes of nested messages
    #[allow(unused_variables)]
    fn compute_size(&self) -> u32 {
        let mut my_size = 0;
        if let Some(ref v) = self.address.as_ref() {
            let len = v.compute_size();
            my_size += 1 + ::protobuf::rt::compute_raw_varint32_size(len) + len;
        }
        for value in &self.topics {
            let len = value.compute_size();
            my_size += 1 + ::protobuf::rt::compute_raw_varint32_size(len) + len;
        };
        if !self.data.is_empty() {
            my_size += ::protobuf::rt::bytes_size(3, &self.data);
        }
        my_size += ::protobuf::rt::unknown_fields_size(self.get_unknown_fields());
        self.cached_size.set(my_size);
        my_size
    }

    fn write_to_with_cached_sizes(&self, os: &mut ::protobuf::CodedOutputStream<'_>) -> ::protobuf::ProtobufResult<()> {
        if let Some(ref v) = self.address.as_ref() {
            os.write_tag(1, ::protobuf::wire_format::WireTypeLengthDelimited)?;
            os.write_raw_varint32(v.get_cached_size())?;
            v.write_to_with_cached_sizes(os)?;
        }
        for v in &self.topics {
            os.write_tag(2, ::protobuf::wire_format::WireTypeLengthDelimited)?;
            os.write_raw_varint32(v.get_cached_size())?;
            v.write_to_with_cached_sizes(os)?;
        };
        if !self.data.is_empty() {
            os.write_bytes(3, &self.data)?;
        }
        os.write_unknown_fields(self.get_unknown_fields())?;
        ::std::result::Result::Ok(())
    }

    fn get_cached_size(&self) -> u32 {
        self.cached_size.get()
    }

    fn get_unknown_fields(&self) -> &::protobuf::UnknownFields {
        &self.unknown_fields
    }

    fn mut_unknown_fields(&mut self) -> &mut ::protobuf::UnknownFields {
        &mut self.unknown_fields
    }

    fn as_any(&self) -> &dyn (::std::any::Any) {
        self as &dyn (::std::any::Any)
    }
    fn as_any_mut(&mut self) -> &mut dyn (::std::any::Any) {
        self as &mut dyn (::std::any::Any)
    }
    fn into_any(self: ::std::boxed::Box<Self>) -> ::std::boxed::Box<dyn (::std::any::Any)> {
        self
    }

    fn descriptor(&self) -> &'static ::protobuf::reflect::MessageDescriptor {
        Self::descriptor_static()
    }

    fn new() -> ResultLog {
        ResultLog::new()
    }

    fn descriptor_static() -> &'static ::protobuf::reflect::MessageDescriptor {
        static descriptor: ::protobuf::rt::LazyV2<::protobuf::reflect::MessageDescriptor> = ::protobuf::rt::LazyV2::INIT;
        descriptor.get(|| {
            let mut fields = ::std::vec::Vec::new();
            fields.push(::protobuf::reflect::accessor::make_singular_ptr_field_accessor::<_, ::protobuf::types::ProtobufTypeMessage<ProtoAddress>>(
                "address",
                |m: &ResultLog| { &m.address },
                |m: &mut ResultLog| { &mut m.address },
            ));
            fields.push(::protobuf::reflect::accessor::make_repeated_field_accessor::<_, ::protobuf::types::ProtobufTypeMessage<RawU256>>(
                "topics",
                |m: &ResultLog| { &m.topics },
                |m: &mut ResultLog| { &mut m.topics },
            ));
            fields.push(::protobuf::reflect::accessor::make_simple_field_accessor::<_, ::protobuf::types::ProtobufTypeBytes>(
                "data",
                |m: &ResultLog| { &m.data },
                |m: &mut ResultLog| { &mut m.data },
            ));
            ::protobuf::reflect::MessageDescriptor::new_pb_name::<ResultLog>(
                "ResultLog",
                fields,
                file_descriptor_proto()
            )
        })
    }

    fn default_instance() -> &'static ResultLog {
        static instance: ::protobuf::rt::LazyV2<ResultLog> = ::protobuf::rt::LazyV2::INIT;
        instance.get(ResultLog::new)
    }
}

impl ::protobuf::Clear for ResultLog {
    fn clear(&mut self) {
        self.address.clear();
        self.topics.clear();
        self.data.clear();
        self.unknown_fields.clear();
    }
}

impl ::std::fmt::Debug for ResultLog {
    fn fmt(&self, f: &mut ::std::fmt::Formatter<'_>) -> ::std::fmt::Result {
        ::protobuf::text_format::fmt(self, f)
    }
}

impl ::protobuf::reflect::ProtobufValue for ResultLog {
    fn as_ref(&self) -> ::protobuf::reflect::ReflectValueRef {
        ::protobuf::reflect::ReflectValueRef::Message(self)
    }
}

static file_descriptor_proto_data: &'static [u8] = b"\
    \n\x11proto_basic.proto\x12\x14top.evm_engine.basic\"$\n\x0cProtoAddress\
    \x12\x14\n\x05value\x18\x01\x20\x01(\x0cR\x05value\"\x1d\n\x07RawU256\
    \x12\x12\n\x04data\x18\x01\x20\x01(\x0cR\x04data\"\x1d\n\x07WeiU256\x12\
    \x12\n\x04data\x18\x01\x20\x01(\x0cR\x04data\"\x94\x01\n\tResultLog\x12<\
    \n\x07address\x18\x01\x20\x01(\x0b2\".top.evm_engine.basic.ProtoAddressR\
    \x07address\x125\n\x06topics\x18\x02\x20\x03(\x0b2\x1d.top.evm_engine.ba\
    sic.RawU256R\x06topics\x12\x12\n\x04data\x18\x03\x20\x01(\x0cR\x04dataJ\
    \x8c\x05\n\x06\x12\x04\0\0\x14\x01\n\x08\n\x01\x0c\x12\x03\0\0\x12\n\x08\
    \n\x01\x02\x12\x03\x02\x08\x1c\n\n\n\x02\x04\0\x12\x04\x04\0\x06\x01\n\n\
    \n\x03\x04\0\x01\x12\x03\x04\x08\x14\n8\n\x04\x04\0\x02\0\x12\x03\x05\
    \x04\x14\"+\x20\x20use\x20bytes\x20to\x20represent\x20H160\x20(\x20[u8;\
    \x2020]\x20)\n\n\r\n\x05\x04\0\x02\0\x04\x12\x04\x05\x04\x04\x16\n\x0c\n\
    \x05\x04\0\x02\0\x05\x12\x03\x05\x04\t\n\x0c\n\x05\x04\0\x02\0\x01\x12\
    \x03\x05\n\x0f\n\x0c\n\x05\x04\0\x02\0\x03\x12\x03\x05\x12\x13\n\n\n\x02\
    \x04\x01\x12\x04\x08\0\n\x01\n\n\n\x03\x04\x01\x01\x12\x03\x08\x08\x0f\n\
    +\n\x04\x04\x01\x02\0\x12\x03\t\x04\x13\"\x1e\x20pub\x20type\x20RawU256\
    \x20=\x20[u8;\x2032];\n\n\r\n\x05\x04\x01\x02\0\x04\x12\x04\t\x04\x08\
    \x11\n\x0c\n\x05\x04\x01\x02\0\x05\x12\x03\t\x04\t\n\x0c\n\x05\x04\x01\
    \x02\0\x01\x12\x03\t\n\x0e\n\x0c\n\x05\x04\x01\x02\0\x03\x12\x03\t\x11\
    \x12\n\n\n\x02\x04\x02\x12\x04\x0c\0\x0e\x01\n\n\n\x03\x04\x02\x01\x12\
    \x03\x0c\x08\x0f\n+\n\x04\x04\x02\x02\0\x12\x03\r\x04\x13\"\x1e\x20pub\
    \x20type\x20WeiU256\x20=\x20[u8;\x2032];\n\n\r\n\x05\x04\x02\x02\0\x04\
    \x12\x04\r\x04\x0c\x11\n\x0c\n\x05\x04\x02\x02\0\x05\x12\x03\r\x04\t\n\
    \x0c\n\x05\x04\x02\x02\0\x01\x12\x03\r\n\x0e\n\x0c\n\x05\x04\x02\x02\0\
    \x03\x12\x03\r\x11\x12\n\n\n\x02\x04\x03\x12\x04\x10\0\x14\x01\n\n\n\x03\
    \x04\x03\x01\x12\x03\x10\x08\x11\n\x0b\n\x04\x04\x03\x02\0\x12\x03\x11\
    \x04\x1d\n\r\n\x05\x04\x03\x02\0\x04\x12\x04\x11\x04\x10\x13\n\x0c\n\x05\
    \x04\x03\x02\0\x06\x12\x03\x11\x04\x10\n\x0c\n\x05\x04\x03\x02\0\x01\x12\
    \x03\x11\x11\x18\n\x0c\n\x05\x04\x03\x02\0\x03\x12\x03\x11\x1b\x1c\n\x0b\
    \n\x04\x04\x03\x02\x01\x12\x03\x12\x04\x20\n\x0c\n\x05\x04\x03\x02\x01\
    \x04\x12\x03\x12\x04\x0c\n\x0c\n\x05\x04\x03\x02\x01\x06\x12\x03\x12\r\
    \x14\n\x0c\n\x05\x04\x03\x02\x01\x01\x12\x03\x12\x15\x1b\n\x0c\n\x05\x04\
    \x03\x02\x01\x03\x12\x03\x12\x1e\x1f\n\x0b\n\x04\x04\x03\x02\x02\x12\x03\
    \x13\x04\x13\n\r\n\x05\x04\x03\x02\x02\x04\x12\x04\x13\x04\x12\x20\n\x0c\
    \n\x05\x04\x03\x02\x02\x05\x12\x03\x13\x04\t\n\x0c\n\x05\x04\x03\x02\x02\
    \x01\x12\x03\x13\n\x0e\n\x0c\n\x05\x04\x03\x02\x02\x03\x12\x03\x13\x11\
    \x12b\x06proto3\
";

static file_descriptor_proto_lazy: ::protobuf::rt::LazyV2<::protobuf::descriptor::FileDescriptorProto> = ::protobuf::rt::LazyV2::INIT;

fn parse_descriptor_proto() -> ::protobuf::descriptor::FileDescriptorProto {
    ::protobuf::Message::parse_from_bytes(file_descriptor_proto_data).unwrap()
}

pub fn file_descriptor_proto() -> &'static ::protobuf::descriptor::FileDescriptorProto {
    file_descriptor_proto_lazy.get(|| {
        parse_descriptor_proto()
    })
}

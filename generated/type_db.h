#pragma once
#include "types.h"

#include <windows.h>
#include <stdint.h>
#include <string>

void register_types( TypeInfo& (*alloc_type) ( void* ), void* alloc_type_param, void* (*alloc_data) ( void*, uint32_t ), void* alloc_data_param );

enum class LocalTypeId : uint32_t;
enum class ObjectTypeId : int;

struct TypeStore;
struct HINSTANCE__;
struct DLLInfo;
struct Appdata;

enum class LocalTypeId : uint32_t
{
    bool_id,
    char_id,
    int_id,
    short_id,
    long_id,
    long_long_id,
    unsigned_int_id,
    unsigned_short_id,
    unsigned_long_id,
    unsigned_long_long_id,
    unsigned_char_id,
    signed_char_id,
    float_id,
    double_id,
    uint_id,
    ushort_id,
    ulong_id,
    uchar_id,
    i8_id,
    i16_id,
    i32_id,
    i64_id,
    u8_id,
    u16_id,
    u32_id,
    u64_id,
    f32_id,
    f64_id,
    std_basic_string_id,
    std_string_id,
    uint32_t_id,
    LocalTypeId_id,
    ObjectTypeId_id,
    TypeStore_id,
    HINSTANCE___id,
    HINSTANCE_id,
    FARPROC_id,
    DLLInfo_id,
    Appdata_id,
    COUNT,
};

enum class ObjectTypeId
{
    Object,
};

template<> constexpr TypeId type_id<bool>() { return { 0, (uint32_t)LocalTypeId::bool_id }; }
constexpr TypeId type_id(const bool& obj) { return type_id<bool>(); }

template<> constexpr TypeId type_id<char>() { return { 0, (uint32_t)LocalTypeId::char_id }; }
constexpr TypeId type_id(const char& obj) { return type_id<char>(); }

template<> constexpr TypeId type_id<int>() { return { 0, (uint32_t)LocalTypeId::int_id }; }
constexpr TypeId type_id(const int& obj) { return type_id<int>(); }

template<> constexpr TypeId type_id<short>() { return { 0, (uint32_t)LocalTypeId::short_id }; }
constexpr TypeId type_id(const short& obj) { return type_id<short>(); }

template<> constexpr TypeId type_id<long>() { return { 0, (uint32_t)LocalTypeId::long_id }; }
constexpr TypeId type_id(const long& obj) { return type_id<long>(); }

template<> constexpr TypeId type_id<long long>() { return { 0, (uint32_t)LocalTypeId::long_long_id }; }
constexpr TypeId type_id(const long long& obj) { return type_id<long long>(); }

template<> constexpr TypeId type_id<unsigned int>() { return { 0, (uint32_t)LocalTypeId::unsigned_int_id }; }
constexpr TypeId type_id(const unsigned int& obj) { return type_id<unsigned int>(); }

template<> constexpr TypeId type_id<unsigned short>() { return { 0, (uint32_t)LocalTypeId::unsigned_short_id }; }
constexpr TypeId type_id(const unsigned short& obj) { return type_id<unsigned short>(); }

template<> constexpr TypeId type_id<unsigned long>() { return { 0, (uint32_t)LocalTypeId::unsigned_long_id }; }
constexpr TypeId type_id(const unsigned long& obj) { return type_id<unsigned long>(); }

template<> constexpr TypeId type_id<unsigned long long>() { return { 0, (uint32_t)LocalTypeId::unsigned_long_long_id }; }
constexpr TypeId type_id(const unsigned long long& obj) { return type_id<unsigned long long>(); }

template<> constexpr TypeId type_id<unsigned char>() { return { 0, (uint32_t)LocalTypeId::unsigned_char_id }; }
constexpr TypeId type_id(const unsigned char& obj) { return type_id<unsigned char>(); }

template<> constexpr TypeId type_id<signed char>() { return { 0, (uint32_t)LocalTypeId::signed_char_id }; }
constexpr TypeId type_id(const signed char& obj) { return type_id<signed char>(); }

template<> constexpr TypeId type_id<float>() { return { 0, (uint32_t)LocalTypeId::float_id }; }
constexpr TypeId type_id(const float& obj) { return type_id<float>(); }

template<> constexpr TypeId type_id<double>() { return { 0, (uint32_t)LocalTypeId::double_id }; }
constexpr TypeId type_id(const double& obj) { return type_id<double>(); }

template<> constexpr TypeId type_id<LocalTypeId>() { return { 0, (uint32_t)LocalTypeId::LocalTypeId_id }; }
constexpr TypeId type_id(const LocalTypeId& obj) { return type_id<LocalTypeId>(); }

template<> constexpr TypeId type_id<ObjectTypeId>() { return { 0, (uint32_t)LocalTypeId::ObjectTypeId_id }; }
constexpr TypeId type_id(const ObjectTypeId& obj) { return type_id<ObjectTypeId>(); }

template<> constexpr TypeId type_id<TypeStore>() { return { 0, (uint32_t)LocalTypeId::TypeStore_id }; }
constexpr TypeId type_id(const TypeStore& obj) { return type_id<TypeStore>(); }

template<> constexpr TypeId type_id<HINSTANCE__>() { return { 0, (uint32_t)LocalTypeId::HINSTANCE___id }; }
constexpr TypeId type_id(const HINSTANCE__& obj) { return type_id<HINSTANCE__>(); }

template<> constexpr TypeId type_id<DLLInfo>() { return { 0, (uint32_t)LocalTypeId::DLLInfo_id }; }
constexpr TypeId type_id(const DLLInfo& obj) { return type_id<DLLInfo>(); }

template<> constexpr TypeId type_id<Appdata>() { return { 0, (uint32_t)LocalTypeId::Appdata_id }; }
constexpr TypeId type_id(const Appdata& obj) { return type_id<Appdata>(); }


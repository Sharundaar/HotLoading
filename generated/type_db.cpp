#include "type_db.h"

#include "basics.h"
#include "basic_types.h"
#include "store.h"
#include "appdata.h"

void register_types( TypeInfo& (*alloc_type) ( void* ), void* alloc_type_param, void* (*alloc_data) ( void*, uint32_t ), void* alloc_data_param )
{
    auto copy_string = [&]( const char* str ) -> const char* {
        auto len = strlen( str );
        if( len == 0 )
            return nullptr;
        char* result = (char*)alloc_data( alloc_data_param, (uint32_t)len+1 );
        strcpy_s( result, len+1, str );
        return result;
    };
    auto alloc_type_short = [&]( TypeInfoType type ) -> TypeInfo& {
        auto& type_info = alloc_type( alloc_type_param );
        type_set_type( type_info, type );
        return type_info;
    };

    auto& type_bool = alloc_type_short( TypeInfoType::Scalar );
    auto& type_char = alloc_type_short( TypeInfoType::Scalar );
    auto& type_int = alloc_type_short( TypeInfoType::Scalar );
    auto& type_short = alloc_type_short( TypeInfoType::Scalar );
    auto& type_long = alloc_type_short( TypeInfoType::Scalar );
    auto& type_long_long = alloc_type_short( TypeInfoType::Scalar );
    auto& type_unsigned_int = alloc_type_short( TypeInfoType::Scalar );
    auto& type_unsigned_short = alloc_type_short( TypeInfoType::Scalar );
    auto& type_unsigned_long = alloc_type_short( TypeInfoType::Scalar );
    auto& type_unsigned_long_long = alloc_type_short( TypeInfoType::Scalar );
    auto& type_unsigned_char = alloc_type_short( TypeInfoType::Scalar );
    auto& type_signed_char = alloc_type_short( TypeInfoType::Scalar );
    auto& type_float = alloc_type_short( TypeInfoType::Scalar );
    auto& type_double = alloc_type_short( TypeInfoType::Scalar );
    auto& type_uint = alloc_type_short( type_unsigned_int.type );
    auto& type_ushort = alloc_type_short( type_unsigned_short.type );
    auto& type_ulong = alloc_type_short( type_unsigned_long.type );
    auto& type_uchar = alloc_type_short( type_unsigned_char.type );
    auto& type_i8 = alloc_type_short( type_signed_char.type );
    auto& type_i16 = alloc_type_short( type_short.type );
    auto& type_i32 = alloc_type_short( type_int.type );
    auto& type_i64 = alloc_type_short( type_long_long.type );
    auto& type_u8 = alloc_type_short( type_unsigned_char.type );
    auto& type_u16 = alloc_type_short( type_unsigned_short.type );
    auto& type_u32 = alloc_type_short( type_unsigned_int.type );
    auto& type_u64 = alloc_type_short( type_unsigned_long_long.type );
    auto& type_f32 = alloc_type_short( type_float.type );
    auto& type_f64 = alloc_type_short( type_double.type );
    auto& type_std_basic_string = alloc_type_short( TypeInfoType::TemplateDef );
    auto& type_std_string = alloc_type_short( TypeInfoType::Typedef );
    auto& type_uint32_t = alloc_type_short( type_unsigned_int.type );
    auto& type_LocalTypeId = alloc_type_short( TypeInfoType::Enum );
    auto& type_ObjectTypeId = alloc_type_short( TypeInfoType::Enum );
    auto& type_TypeStore = alloc_type_short( TypeInfoType::Struct );
    auto& type_HINSTANCE__ = alloc_type_short( TypeInfoType::Struct );
    auto& type_HINSTANCE = alloc_type_short( TypeInfoType::Typedef );
    auto& type_FARPROC = alloc_type_short( TypeInfoType::Typedef );
    auto& type_DLLInfo = alloc_type_short( TypeInfoType::Struct );
    auto& type_Appdata = alloc_type_short( TypeInfoType::Struct );

    type_set_name( type_bool, copy_string( "bool" ) );
    type_set_id( type_bool, type_id<bool>() );
    scalar_set_size( type_bool.scalar_info, sizeof( bool ) );
    scalar_set_type( type_bool.scalar_info, ScalarInfoType::BOOL );

    type_set_name( type_char, copy_string( "char" ) );
    type_set_id( type_char, type_id<char>() );
    scalar_set_size( type_char.scalar_info, sizeof( char ) );
    scalar_set_type( type_char.scalar_info, ScalarInfoType::CHAR );

    type_set_name( type_int, copy_string( "int" ) );
    type_set_id( type_int, type_id<int>() );
    scalar_set_size( type_int.scalar_info, sizeof( int ) );
    scalar_set_type( type_int.scalar_info, ScalarInfoType::INT );

    type_set_name( type_short, copy_string( "short" ) );
    type_set_id( type_short, type_id<short>() );
    scalar_set_size( type_short.scalar_info, sizeof( short ) );
    scalar_set_type( type_short.scalar_info, ScalarInfoType::INT );

    type_set_name( type_long, copy_string( "long" ) );
    type_set_id( type_long, type_id<long>() );
    scalar_set_size( type_long.scalar_info, sizeof( long ) );
    scalar_set_type( type_long.scalar_info, ScalarInfoType::INT );

    type_set_name( type_long_long, copy_string( "long long" ) );
    type_set_id( type_long_long, type_id<long long>() );
    scalar_set_size( type_long_long.scalar_info, sizeof( long long ) );
    scalar_set_type( type_long_long.scalar_info, ScalarInfoType::INT );

    type_set_name( type_unsigned_int, copy_string( "unsigned int" ) );
    type_set_id( type_unsigned_int, type_id<unsigned int>() );
    scalar_set_size( type_unsigned_int.scalar_info, sizeof( unsigned int ) );
    scalar_set_type( type_unsigned_int.scalar_info, ScalarInfoType::UINT );

    type_set_name( type_unsigned_short, copy_string( "unsigned short" ) );
    type_set_id( type_unsigned_short, type_id<unsigned short>() );
    scalar_set_size( type_unsigned_short.scalar_info, sizeof( unsigned short ) );
    scalar_set_type( type_unsigned_short.scalar_info, ScalarInfoType::UINT );

    type_set_name( type_unsigned_long, copy_string( "unsigned long" ) );
    type_set_id( type_unsigned_long, type_id<unsigned long>() );
    scalar_set_size( type_unsigned_long.scalar_info, sizeof( unsigned long ) );
    scalar_set_type( type_unsigned_long.scalar_info, ScalarInfoType::UINT );

    type_set_name( type_unsigned_long_long, copy_string( "unsigned long long" ) );
    type_set_id( type_unsigned_long_long, type_id<unsigned long long>() );
    scalar_set_size( type_unsigned_long_long.scalar_info, sizeof( unsigned long long ) );
    scalar_set_type( type_unsigned_long_long.scalar_info, ScalarInfoType::UINT );

    type_set_name( type_unsigned_char, copy_string( "unsigned char" ) );
    type_set_id( type_unsigned_char, type_id<unsigned char>() );
    scalar_set_size( type_unsigned_char.scalar_info, sizeof( unsigned char ) );
    scalar_set_type( type_unsigned_char.scalar_info, ScalarInfoType::CHAR );

    type_set_name( type_signed_char, copy_string( "signed char" ) );
    type_set_id( type_signed_char, type_id<signed char>() );
    scalar_set_size( type_signed_char.scalar_info, sizeof( signed char ) );
    scalar_set_type( type_signed_char.scalar_info, ScalarInfoType::CHAR );

    type_set_name( type_float, copy_string( "float" ) );
    type_set_id( type_float, type_id<float>() );
    scalar_set_size( type_float.scalar_info, sizeof( float ) );
    scalar_set_type( type_float.scalar_info, ScalarInfoType::FLOAT );

    type_set_name( type_double, copy_string( "double" ) );
    type_set_id( type_double, type_id<double>() );
    scalar_set_size( type_double.scalar_info, sizeof( double ) );
    scalar_set_type( type_double.scalar_info, ScalarInfoType::FLOAT );

    type_uint = type_unsigned_int;
    type_set_name( type_uint, copy_string( "uint" ) );
    type_set_id( type_uint, { 0, (uint32_t)LocalTypeId::uint_id } );

    type_ushort = type_unsigned_short;
    type_set_name( type_ushort, copy_string( "ushort" ) );
    type_set_id( type_ushort, { 0, (uint32_t)LocalTypeId::ushort_id } );

    type_ulong = type_unsigned_long;
    type_set_name( type_ulong, copy_string( "ulong" ) );
    type_set_id( type_ulong, { 0, (uint32_t)LocalTypeId::ulong_id } );

    type_uchar = type_unsigned_char;
    type_set_name( type_uchar, copy_string( "uchar" ) );
    type_set_id( type_uchar, { 0, (uint32_t)LocalTypeId::uchar_id } );

    type_i8 = type_signed_char;
    type_set_name( type_i8, copy_string( "i8" ) );
    type_set_id( type_i8, { 0, (uint32_t)LocalTypeId::i8_id } );

    type_i16 = type_short;
    type_set_name( type_i16, copy_string( "i16" ) );
    type_set_id( type_i16, { 0, (uint32_t)LocalTypeId::i16_id } );

    type_i32 = type_int;
    type_set_name( type_i32, copy_string( "i32" ) );
    type_set_id( type_i32, { 0, (uint32_t)LocalTypeId::i32_id } );

    type_i64 = type_long_long;
    type_set_name( type_i64, copy_string( "i64" ) );
    type_set_id( type_i64, { 0, (uint32_t)LocalTypeId::i64_id } );

    type_u8 = type_unsigned_char;
    type_set_name( type_u8, copy_string( "u8" ) );
    type_set_id( type_u8, { 0, (uint32_t)LocalTypeId::u8_id } );

    type_u16 = type_unsigned_short;
    type_set_name( type_u16, copy_string( "u16" ) );
    type_set_id( type_u16, { 0, (uint32_t)LocalTypeId::u16_id } );

    type_u32 = type_unsigned_int;
    type_set_name( type_u32, copy_string( "u32" ) );
    type_set_id( type_u32, { 0, (uint32_t)LocalTypeId::u32_id } );

    type_u64 = type_unsigned_long_long;
    type_set_name( type_u64, copy_string( "u64" ) );
    type_set_id( type_u64, { 0, (uint32_t)LocalTypeId::u64_id } );

    type_f32 = type_float;
    type_set_name( type_f32, copy_string( "f32" ) );
    type_set_id( type_f32, { 0, (uint32_t)LocalTypeId::f32_id } );

    type_f64 = type_double;
    type_set_name( type_f64, copy_string( "f64" ) );
    type_set_id( type_f64, { 0, (uint32_t)LocalTypeId::f64_id } );

    type_set_name( type_std_basic_string, copy_string( "std::basic_string" ) );
    type_set_id( type_std_basic_string, { 0, (uint32_t)LocalTypeId::std_basic_string_id } );
    auto type_std_basic_string_instances = (TypeInfo*)alloc_data( alloc_data_param, sizeof(TypeInfo) * 1 );
    template_set_instances( type_std_basic_string, type_std_basic_string_instances, 1 );
    type_set_type( type_std_basic_string_instances[0], TypeInfoType::TemplateInst );
    type_set_name( type_std_basic_string_instances[0], type_std_basic_string.name );
    type_set_id( type_std_basic_string_instances[0], type_std_basic_string.type_id );
    template_instance_set_params( &type_std_basic_string_instances[0].template_inst_info, (TemplateParam*)alloc_data( alloc_data_param, sizeof(TemplateParam) * 3 ), 3 );
    type_std_basic_string_instances[0].template_inst_info.definition = &type_std_basic_string;
    type_std_basic_string_instances[0].template_inst_info.definition = &type_std_basic_string;
    field_set_modifiers( type_std_basic_string_instances[0].template_inst_info.params[0].info, (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_type( type_std_basic_string_instances[0].template_inst_info.params[0].info, &type_char );
    field_set_modifiers( type_std_basic_string_instances[0].template_inst_info.params[1].info, (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_modifiers( type_std_basic_string_instances[0].template_inst_info.params[2].info, (FieldInfoModifier) (FieldInfoModifier::NONE) );

    type_set_name( type_std_string, copy_string( "std::string" ) );
    type_set_id( type_std_string, { 0, (uint32_t)LocalTypeId::std_string_id } );
    field_set_name( type_std_string.typedef_info.info, copy_string( "basic_string" ) );
    field_set_modifiers( type_std_string.typedef_info.info, (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_type( type_std_string.typedef_info.info, &type_std_basic_string.template_def_info.instances[0] );

    {
        auto& type__assert = *((TypeInfo*)alloc_data( alloc_data_param, sizeof( TypeInfo ) ));
        type_set_type( type__assert, TypeInfoType::Function );
        type__assert = {};
        type_set_name( type__assert, copy_string( "_assert" ) );
        FieldInfo type__assert_return_type;
        field_set_modifiers( type__assert_return_type, (FieldInfoModifier) (FieldInfoModifier::NONE) );
        field_set_type( type__assert_return_type, &type_bool );
        func_set_return_type( type__assert, type__assert_return_type );
        auto* type__assert_params = (FieldInfo*)alloc_data( alloc_data_param, sizeof(FieldInfo) * 2 );
        {
            field_set_name( type__assert_params[0], copy_string( "condition" ) );
            field_set_modifiers( type__assert_params[0], (FieldInfoModifier) (FieldInfoModifier::NONE) );
            field_set_type( type__assert_params[0], &type_bool );
        }
        {
            field_set_name( type__assert_params[1], copy_string( "msg" ) );
            field_set_modifiers( type__assert_params[1], (FieldInfoModifier) (FieldInfoModifier::CONSTANT | FieldInfoModifier::REFERENCE) );
            field_set_type( type__assert_params[1], &type_std_string );
        }
        func_set_parameters( type__assert, type__assert_params, 2 );
    }

    {
        auto& type_print = *((TypeInfo*)alloc_data( alloc_data_param, sizeof( TypeInfo ) ));
        type_set_type( type_print, TypeInfoType::Function );
        type_print = {};
        type_set_name( type_print, copy_string( "print" ) );
        auto* type_print_params = (FieldInfo*)alloc_data( alloc_data_param, sizeof(FieldInfo) * 1 );
        {
            field_set_name( type_print_params[0], copy_string( "str" ) );
            field_set_modifiers( type_print_params[0], (FieldInfoModifier) (FieldInfoModifier::CONSTANT | FieldInfoModifier::REFERENCE) );
            field_set_type( type_print_params[0], &type_std_string );
        }
        func_set_parameters( type_print, type_print_params, 1 );
    }

    {
        auto& type_println = *((TypeInfo*)alloc_data( alloc_data_param, sizeof( TypeInfo ) ));
        type_set_type( type_println, TypeInfoType::Function );
        type_println = {};
        type_set_name( type_println, copy_string( "println" ) );
        auto* type_println_params = (FieldInfo*)alloc_data( alloc_data_param, sizeof(FieldInfo) * 1 );
        {
            field_set_name( type_println_params[0], copy_string( "str" ) );
            field_set_modifiers( type_println_params[0], (FieldInfoModifier) (FieldInfoModifier::CONSTANT | FieldInfoModifier::REFERENCE) );
            field_set_type( type_println_params[0], &type_std_string );
        }
        func_set_parameters( type_println, type_println_params, 1 );
    }

    {
        auto& type_working_directory = *((TypeInfo*)alloc_data( alloc_data_param, sizeof( TypeInfo ) ));
        type_set_type( type_working_directory, TypeInfoType::Function );
        type_working_directory = {};
        type_set_name( type_working_directory, copy_string( "working_directory" ) );
        FieldInfo type_working_directory_return_type;
        field_set_modifiers( type_working_directory_return_type, (FieldInfoModifier) (FieldInfoModifier::NONE) );
        field_set_type( type_working_directory_return_type, &type_std_string );
        func_set_return_type( type_working_directory, type_working_directory_return_type );
    }

    {
        auto& type_register_types = *((TypeInfo*)alloc_data( alloc_data_param, sizeof( TypeInfo ) ));
        type_set_type( type_register_types, TypeInfoType::Function );
        type_register_types = {};
        type_set_name( type_register_types, copy_string( "register_types" ) );
        auto* type_register_types_params = (FieldInfo*)alloc_data( alloc_data_param, sizeof(FieldInfo) * 4 );
        {
            field_set_name( type_register_types_params[0], copy_string( "alloc_type" ) );
            field_set_modifiers( type_register_types_params[0], (FieldInfoModifier) (FieldInfoModifier::POINTER) );
        }
        {
            field_set_name( type_register_types_params[1], copy_string( "alloc_type_param" ) );
            field_set_modifiers( type_register_types_params[1], (FieldInfoModifier) (FieldInfoModifier::POINTER) );
        }
        {
            field_set_name( type_register_types_params[2], copy_string( "alloc_data" ) );
            field_set_modifiers( type_register_types_params[2], (FieldInfoModifier) (FieldInfoModifier::POINTER) );
        }
        {
            field_set_name( type_register_types_params[3], copy_string( "alloc_data_param" ) );
            field_set_modifiers( type_register_types_params[3], (FieldInfoModifier) (FieldInfoModifier::POINTER) );
        }
        func_set_parameters( type_register_types, type_register_types_params, 4 );
    }

    type_uint32_t = type_unsigned_int;
    type_set_name( type_uint32_t, copy_string( "uint32_t" ) );
    type_set_id( type_uint32_t, { 0, (uint32_t)LocalTypeId::uint32_t_id } );

    type_set_name( type_LocalTypeId, copy_string( "LocalTypeId" ) );
    type_set_id( type_LocalTypeId, type_id<LocalTypeId>() );
    enum_set_underlying_type( type_LocalTypeId.enum_info, &type_uint32_t );
    {
        auto values = (EnumValue*)alloc_data( alloc_data_param, sizeof(EnumValue) * 40 );
        values[0] = { copy_string( "i64_id" ), 21 };
        values[1] = { copy_string( "TypeStore_id" ), 33 };
        values[2] = { copy_string( "DLLInfo_id" ), 37 };
        values[3] = { copy_string( "HINSTANCE_id" ), 35 };
        values[4] = { copy_string( "short_id" ), 3 };
        values[5] = { copy_string( "u8_id" ), 22 };
        values[6] = { copy_string( "u64_id" ), 25 };
        values[7] = { copy_string( "Appdata_id" ), 38 };
        values[8] = { copy_string( "unsigned_char_id" ), 10 };
        values[9] = { copy_string( "ObjectTypeId_id" ), 32 };
        values[10] = { copy_string( "uint_id" ), 14 };
        values[11] = { copy_string( "i16_id" ), 19 };
        values[12] = { copy_string( "u32_id" ), 24 };
        values[13] = { copy_string( "std_string_id" ), 29 };
        values[14] = { copy_string( "char_id" ), 1 };
        values[15] = { copy_string( "f32_id" ), 26 };
        values[16] = { copy_string( "HINSTANCE___id" ), 34 };
        values[17] = { copy_string( "FARPROC_id" ), 36 };
        values[18] = { copy_string( "ushort_id" ), 15 };
        values[19] = { copy_string( "std_basic_string_id" ), 28 };
        values[20] = { copy_string( "unsigned_int_id" ), 6 };
        values[21] = { copy_string( "float_id" ), 12 };
        values[22] = { copy_string( "signed_char_id" ), 11 };
        values[23] = { copy_string( "i8_id" ), 18 };
        values[24] = { copy_string( "double_id" ), 13 };
        values[25] = { copy_string( "int_id" ), 2 };
        values[26] = { copy_string( "COUNT" ), 39 };
        values[27] = { copy_string( "i32_id" ), 20 };
        values[28] = { copy_string( "LocalTypeId_id" ), 31 };
        values[29] = { copy_string( "uint32_t_id" ), 30 };
        values[30] = { copy_string( "long_long_id" ), 5 };
        values[31] = { copy_string( "unsigned_long_id" ), 8 };
        values[32] = { copy_string( "unsigned_long_long_id" ), 9 };
        values[33] = { copy_string( "unsigned_short_id" ), 7 };
        values[34] = { copy_string( "long_id" ), 4 };
        values[35] = { copy_string( "ulong_id" ), 16 };
        values[36] = { copy_string( "f64_id" ), 27 };
        values[37] = { copy_string( "uchar_id" ), 17 };
        values[38] = { copy_string( "u16_id" ), 23 };
        values[39] = { copy_string( "bool_id" ), 0 };
        enum_set_values( type_LocalTypeId.enum_info, values, 40 );
    }

    type_set_name( type_ObjectTypeId, copy_string( "ObjectTypeId" ) );
    type_set_id( type_ObjectTypeId, type_id<ObjectTypeId>() );
    enum_set_underlying_type( type_ObjectTypeId.enum_info, &type_int );
    {
        auto values = (EnumValue*)alloc_data( alloc_data_param, sizeof(EnumValue) * 1 );
        values[0] = { copy_string( "Object" ), 0 };
        enum_set_values( type_ObjectTypeId.enum_info, values, 1 );
    }

    {
        auto& type_type_id = *((TypeInfo*)alloc_data( alloc_data_param, sizeof( TypeInfo ) ));
        type_set_type( type_type_id, TypeInfoType::Function );
        type_type_id = {};
        type_set_name( type_type_id, copy_string( "type_id" ) );
        FieldInfo type_type_id_return_type;
        field_set_modifiers( type_type_id_return_type, (FieldInfoModifier) (FieldInfoModifier::NONE) );
        field_set_type( type_type_id_return_type, &type_int );
        func_set_return_type( type_type_id, type_type_id_return_type );
    }

    type_set_name( type_TypeStore, copy_string( "TypeStore" ) );
    type_set_id( type_TypeStore, type_id<TypeStore>() );
    struct_set_size( type_TypeStore, sizeof( TypeStore ) );
    auto type_TypeStore_fields = (FieldInfo*)alloc_data( alloc_data_param, sizeof(FieldInfo) * 5 );
    field_set_name( type_TypeStore_fields[0], copy_string( "buffer" ) );
    field_set_modifiers( type_TypeStore_fields[0], (FieldInfoModifier) (FieldInfoModifier::POINTER) );
    field_set_type( type_TypeStore_fields[0], &type_u8 );
    field_set_name( type_TypeStore_fields[1], copy_string( "buffer_capacity" ) );
    field_set_modifiers( type_TypeStore_fields[1], (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_type( type_TypeStore_fields[1], &type_u32 );
    field_set_name( type_TypeStore_fields[2], copy_string( "buffer_size" ) );
    field_set_modifiers( type_TypeStore_fields[2], (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_type( type_TypeStore_fields[2], &type_u32 );
    field_set_name( type_TypeStore_fields[3], copy_string( "type_count" ) );
    field_set_modifiers( type_TypeStore_fields[3], (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_type( type_TypeStore_fields[3], &type_u32 );
    field_set_name( type_TypeStore_fields[4], copy_string( "all_types" ) );
    field_set_modifiers( type_TypeStore_fields[4], (FieldInfoModifier) (FieldInfoModifier::NONE) );
    struct_set_fields( type_TypeStore, type_TypeStore_fields, 5 );

    {
        auto& type_init_type_store = *((TypeInfo*)alloc_data( alloc_data_param, sizeof( TypeInfo ) ));
        type_set_type( type_init_type_store, TypeInfoType::Function );
        type_init_type_store = {};
        type_set_name( type_init_type_store, copy_string( "init_type_store" ) );
        FieldInfo type_init_type_store_return_type;
        field_set_modifiers( type_init_type_store_return_type, (FieldInfoModifier) (FieldInfoModifier::NONE) );
        field_set_type( type_init_type_store_return_type, &type_TypeStore );
        func_set_return_type( type_init_type_store, type_init_type_store_return_type );
    }

    {
        auto& type_cleanup_type_store = *((TypeInfo*)alloc_data( alloc_data_param, sizeof( TypeInfo ) ));
        type_set_type( type_cleanup_type_store, TypeInfoType::Function );
        type_cleanup_type_store = {};
        type_set_name( type_cleanup_type_store, copy_string( "cleanup_type_store" ) );
        auto* type_cleanup_type_store_params = (FieldInfo*)alloc_data( alloc_data_param, sizeof(FieldInfo) * 1 );
        {
            field_set_name( type_cleanup_type_store_params[0], copy_string( "store" ) );
            field_set_modifiers( type_cleanup_type_store_params[0], (FieldInfoModifier) (FieldInfoModifier::REFERENCE) );
            field_set_type( type_cleanup_type_store_params[0], &type_TypeStore );
        }
        func_set_parameters( type_cleanup_type_store, type_cleanup_type_store_params, 1 );
    }

    type_set_name( type_HINSTANCE__, copy_string( "HINSTANCE__" ) );
    type_set_id( type_HINSTANCE__, type_id<HINSTANCE__>() );
    struct_set_size( type_HINSTANCE__, sizeof( HINSTANCE__ ) );
    auto type_HINSTANCE___fields = (FieldInfo*)alloc_data( alloc_data_param, sizeof(FieldInfo) * 1 );
    field_set_name( type_HINSTANCE___fields[0], copy_string( "unused" ) );
    field_set_modifiers( type_HINSTANCE___fields[0], (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_type( type_HINSTANCE___fields[0], &type_int );
    struct_set_fields( type_HINSTANCE__, type_HINSTANCE___fields, 1 );

    type_set_name( type_HINSTANCE, copy_string( "HINSTANCE" ) );
    type_set_id( type_HINSTANCE, { 0, (uint32_t)LocalTypeId::HINSTANCE_id } );
    field_set_modifiers( type_HINSTANCE.typedef_info.info, (FieldInfoModifier) (FieldInfoModifier::POINTER) );

    type_set_name( type_FARPROC, copy_string( "FARPROC" ) );
    type_set_id( type_FARPROC, { 0, (uint32_t)LocalTypeId::FARPROC_id } );
    field_set_modifiers( type_FARPROC.typedef_info.info, (FieldInfoModifier) (FieldInfoModifier::POINTER) );

    type_set_name( type_DLLInfo, copy_string( "DLLInfo" ) );
    type_set_id( type_DLLInfo, type_id<DLLInfo>() );
    struct_set_size( type_DLLInfo, sizeof( DLLInfo ) );
    auto type_DLLInfo_fields = (FieldInfo*)alloc_data( alloc_data_param, sizeof(FieldInfo) * 2 );
    field_set_name( type_DLLInfo_fields[0], copy_string( "instance" ) );
    field_set_modifiers( type_DLLInfo_fields[0], (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_type( type_DLLInfo_fields[0], &type_HINSTANCE );
    field_set_name( type_DLLInfo_fields[1], copy_string( "loop_func" ) );
    field_set_offset( type_DLLInfo_fields[1], 8 );
    field_set_modifiers( type_DLLInfo_fields[1], (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_type( type_DLLInfo_fields[1], &type_FARPROC );
    struct_set_fields( type_DLLInfo, type_DLLInfo_fields, 2 );

    type_set_name( type_Appdata, copy_string( "Appdata" ) );
    type_set_id( type_Appdata, type_id<Appdata>() );
    struct_set_size( type_Appdata, sizeof( Appdata ) );
    auto type_Appdata_fields = (FieldInfo*)alloc_data( alloc_data_param, sizeof(FieldInfo) * 2 );
    field_set_name( type_Appdata_fields[0], copy_string( "type_store" ) );
    field_set_modifiers( type_Appdata_fields[0], (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_type( type_Appdata_fields[0], &type_TypeStore );
    field_set_name( type_Appdata_fields[1], copy_string( "dll_info" ) );
    field_set_modifiers( type_Appdata_fields[1], (FieldInfoModifier) (FieldInfoModifier::NONE) );
    field_set_type( type_Appdata_fields[1], &type_DLLInfo );
    struct_set_fields( type_Appdata, type_Appdata_fields, 2 );

}


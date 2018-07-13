#include "inspector.h"

#include "type_db.h"
#include "imgui.h"
#include "mathlib.h"
#include "math_helper.h"

void draw_scalar_inspector( const char* name, const TypeInfo* type, u8* data )
{
    assert( type && type->type == TypeInfoType::Scalar, "type is not a scalar type." );

    const ScalarInfo& scalar_info = type->scalar_info;

    switch( scalar_info.scalar_type )
    {
        case ScalarInfoType::BOOL:
        {
            bool* data_bool = reinterpret_cast<bool*>(data);
            ImGui::Checkbox( name, data_bool );
            break;
        }
        case ScalarInfoType::FLOAT:
        {
            if( scalar_info.size == sizeof( f32 ) )
            {
                f32* data_float = reinterpret_cast<f32*>(data);
                ImGui::InputFloat( name, data_float, 0.1f, 1.0f, 3 );
            }
            else if( scalar_info.size == sizeof( f64 ) )
            {
                f64* data_float = reinterpret_cast<f64*>(data);
                ImGui::InputDouble( name, data_float, 0.1, 1.0 );
            }
            else
            {
                assert( false, "Only handling float and double edit." );
            }
            break;
        }
        case ScalarInfoType::CHAR:
        {
            unimplemented();
            break;
        }
        case ScalarInfoType::INT:
        {
            switch( scalar_info.size )
            {
                case sizeof(i8):
                {
                    i8* data_int = reinterpret_cast<i8*>( data );
                    i64 temp = *data_int;
                    if( ImGui::InputScalar( name, ImGuiDataType_S64, &temp ) )
                        *data_int = (i8) temp;
                    break;
                }
                case sizeof(i16):
                {
                    i16* data_int = reinterpret_cast<i16*>( data );
                    i64 temp = *data_int;
                    if( ImGui::InputScalar( name, ImGuiDataType_S64, &temp ) )
                        *data_int = (i16) temp;
                    break;
                }
                case sizeof(i32):
                {
                    i32* data_int = reinterpret_cast<i32*>( data );
                    i64 temp = *data_int;
                    if( ImGui::InputScalar( name, ImGuiDataType_S64, &temp ) )
                        *data_int = (i32) temp;
                    break;
                }
                case sizeof(i64):
                {
                    i64* data_int = reinterpret_cast<i64*>( data );
                    i64 temp = *data_int;
                    if( ImGui::InputScalar( name, ImGuiDataType_S64, &temp ) )
                        *data_int = (i64) temp;
                    break;
                }
                default:
                    assert( false, "Only handling i8, i16, i32 or i64." );
                    break;
            }
            break;
        }
        case ScalarInfoType::UINT:
        {
            switch( scalar_info.size )
            {
                case sizeof(u8):
                {
                    u8* data_int = reinterpret_cast<u8*>( data );
                    u64 temp = *data_int;
                    if( ImGui::InputScalar( name, ImGuiDataType_U64, &temp ) )
                        *data_int = (u8) temp;
                    break;
                }
                case sizeof(u16):
                {
                    u16* data_int = reinterpret_cast<u16*>( data );
                    u64 temp = *data_int;
                    if( ImGui::InputScalar( name, ImGuiDataType_U64, &temp ) )
                        *data_int = (u16) temp;
                    break;
                }
                case sizeof(u32):
                {
                    u32* data_int = reinterpret_cast<u32*>( data );
                    u64 temp = *data_int;
                    if( ImGui::InputScalar( name, ImGuiDataType_U64, &temp ) )
                        *data_int = (u32) temp;
                    break;
                }
                case sizeof(u64):
                {
                    u64* data_int = reinterpret_cast<u64*>( data );
                    u64 temp = *data_int;
                    if( ImGui::InputScalar( name, ImGuiDataType_U64, &temp ) )
                        *data_int = (u64) temp;
                    break;
                }
                default:
                    assert( false, "Only handling u8, u16, u32 or u64." );
                    break;
            }
            break;
        }
        default:
            unimplemented();
            break;

    }

}

// @Cleanup: This leaks on dll reload...
static u32 s_string_buffer_capacity;
static char* s_string_buffer;
bool draw_data_inspector_override( const char* name,const TypeInfo* type, u8* data )
{
    constexpr TypeId string_id = static_cast<TypeId>( LocalTypeId::std_string_id );

    switch( type->type_id )
    {
        case string_id:
        {
            auto data_string = (std::string*)data;

            if( s_string_buffer_capacity <= data_string->capacity() )
            {
                if( s_string_buffer )
                    delete[] s_string_buffer;
                s_string_buffer_capacity = (u32)max( 16, data_string->capacity() * 2 );
                s_string_buffer = new char[s_string_buffer_capacity];
            }

            strcpy_s( s_string_buffer, s_string_buffer_capacity, data_string->c_str() );
            ImGui::InputText( name, s_string_buffer, s_string_buffer_capacity );
            data_string->assign( s_string_buffer );

            return true;
        }
        case type_id<Vector3>():
        {
            if( ImGui::TreeNode( name ) )
            {
                auto data_vector = (Vector3*)data;
                f32 data_float = data_vector->x;
                if( ImGui::SliderFloat("X", &data_float, -1.0f, 1.0f) )
                    data_vector->x = data_float;
                data_float = data_vector->y;
                if( ImGui::SliderFloat("Y", &data_float, -1.0f, 1.0f) )
                    data_vector->y = data_float;
                data_float = data_vector->z;
                if( ImGui::SliderFloat("Z", &data_float, -1.0f, 1.0f) )
                    data_vector->z = data_float;
                ImGui::TreePop();
            }
            return true;
        }
        case type_id<Quaternion>():
        {
            if(ImGui::TreeNode( name ) )
            {
                auto data_quaternion = (Quaternion*)data;
                Vector3 ypr = Quaternion::ToEulerAngle( *data_quaternion ) * (180.0f / M_PI);
                bool change = false;
                f32 data_float = ypr.x;
                if( ImGui::SliderFloat("Yaw", &data_float, -180.0f, 180.0) )
                {
                    ypr.x = data_float;
                    change = true;
                }
                data_float = ypr.y;
                if( ImGui::SliderFloat("Pitch", &data_float, -89.0f, 89.0f) )
                {
                    ypr.y = data_float;
                    change = true;
                }
                data_float = ypr.z;
                if( ImGui::SliderFloat("Roll", &data_float, -180.0f, 180.0f) )
                {
                    ypr.z = data_float;
                    change = true;
                }
                ypr = ypr * (M_PI / 180.0f);
                if( change )
                    *data_quaternion = Quaternion::FromEulerAngle( ypr.x, ypr.y, ypr.z );
                ImGui::TreePop();
            }
            return true;
        }
    }
    return false;
}

void draw_struct_inspector( const char* name, const TypeInfo* type, u8* data )
{
    assert( type && type->type == TypeInfoType::Struct, "Only handling struct type in the inspector." );

    const StructInfo& struct_info = type->struct_info;
    for( uint i=0; i<struct_info.field_count; ++i )
    {
        const auto& field = struct_info.fields[i];
        if( field.type )
        {
            if( (field.modifier & FieldInfoModifier::POINTER) && !(field.modifier & FieldInfoModifier::REFERENCE) )
                ImGui::Text( "%s* %s", field.type->name, field.name );
            else if( !(field.modifier & FieldInfoModifier::POINTER) && (field.modifier & FieldInfoModifier::REFERENCE) )
                ImGui::Text( "%s& %s", field.type->name, field.name );
            else if( (field.modifier & FieldInfoModifier::POINTER) && (field.modifier & FieldInfoModifier::REFERENCE) )
                ImGui::Text( "%s ambiguous %s", field.type->name, field.name );
            else if( !(field.modifier & FieldInfoModifier::POINTER) && !(field.modifier & FieldInfoModifier::REFERENCE) )
            {
                draw_data_inspector( field.name, field.type, data + field.offset );
            }
        }
        else
        {
            ImGui::Text( "(unknown) %s", field.name );
        }
    }
}

void draw_data_inspector( const char* name, const TypeInfo* type, u8* data )
{
    if( !type )
    {
        ImGui::Text( "(unknown) %s", type->name, name );
        return;
    }

    if( !draw_data_inspector_override( name, type, data ) )
    {
        switch( type->type )
        {
            case TypeInfoType::Scalar:
                draw_scalar_inspector( name, type, data );
                break;
            case TypeInfoType::Struct:
                if( ImGui::TreeNode( name ) )
                {
                    draw_struct_inspector( name, type, data );
                    ImGui::TreePop();
                }
                break;
            default:
                if(type)
                    ImGui::Text( "%s %s", type->name, name );
                break;
        }
    }
}
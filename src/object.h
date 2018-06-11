#pragma once

#include "dll.h"

#include "types.h"
#include "appdata.h"

#define GENERATE_BODY( Struct ) \
    Struct();                   \
    Struct( TypeId _type_id );     \
    Object* operator()() { return (Object*) this; }

struct Object
{
    Object( TypeId _type_id ) : m_type_id( _type_id ) {}
    TypeId m_type_id;
    const TypeInfo* get_type() { return get_dll_appdata().metadata.type_infos[m_type_id.local_type]; }
};

template<typename T>
T* cast( Object* obj )
{
    if( type_of<T>() == obj->get_type() )
    {
        return static_cast<T*>( obj );
    }

    return nullptr;
}

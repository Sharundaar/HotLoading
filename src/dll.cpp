#include "dll.h"

#include "basics.h"

#define TYPES_IMPLEMENTATION
#include "types.h"
#undef TYPES_IMPLEMENTATION

Appdata  s_default_appdata = {};
Appdata* s_appdata = nullptr;

DLLEXPORT void set_dll_appdata( Appdata* appdata )
{
    s_appdata = appdata;
}

DLLEXPORT Appdata& get_dll_appdata()
{
    if( s_appdata )
        return *s_appdata;
    else
        return s_default_appdata;
}

DLLEXPORT void loop( Appdata& appdata )
{
    println("DLL blablabla reload count: %", appdata.dll_info.reload_count);
}

TypeStore get_dll_types()
{
    TypeStore store = {};

    u32 capacity = 0x7000;
    store.buffer = new u8[capacity];
    store.buffer_capacity = capacity;

    auto alloc_type = []( void* param ) -> TypeInfo& {
        auto store = (TypeStore*)param;
        assert( store->buffer_size + sizeof(TypeInfo) <= store->buffer_capacity, "store capacity is too low." );
        auto alloc = new (store->buffer + store->buffer_size) TypeInfo();
        store->buffer_size += sizeof(TypeInfo);
        store->type_count++;
        return *alloc;
    };

    auto alloc_data = []( void* param, uint32_t size ) -> void* {
        auto store = (TypeStore*)param;
        assert( store->buffer_size + size <= store->buffer_capacity, "store capacity is too low." );
        auto alloc = (void*)(store->buffer + store->buffer_size);
        store->buffer_size += size;
        return alloc;
    };

    register_types( alloc_type, &store, alloc_data, &store );

    for(u32 idx = 0; idx < store.type_count; ++idx)
    {
        auto type = (TypeInfo*) (store.buffer+(idx*sizeof(TypeInfo)));
        if( type->type_id.local_type != INVALID_TYPE_ID )
            store.all_types[type->type_id.local_type] = type;
    }

    return store;
}
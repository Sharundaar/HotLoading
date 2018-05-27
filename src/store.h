#pragma once

#include "basics.h"
#include "types.h"
#include "type_db.h"

struct TypeStore
{
    u8* buffer          = nullptr;
    u32 buffer_capacity = 0;
    u32 buffer_size     = 0;
    u32 type_count      = 0;
    const TypeInfo* all_types[(u32)LocalTypeId::COUNT] = { nullptr };
};

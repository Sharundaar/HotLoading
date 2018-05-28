#pragma once

#include "basics.h"
#include "types.h"

struct TypeStore
{
    u8* buffer          = nullptr;
    u32 buffer_capacity = 0;
    u32 buffer_size     = 0;
    u32 type_count      = 0;
    const TypeInfo** all_types = nullptr;
};

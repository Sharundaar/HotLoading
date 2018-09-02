#pragma once

#include "basic_types.h"
#include "types.h"

void draw_scalar_inspector( const char* name, const TypeInfo* type, u8* data );
bool draw_data_inspector_override( const char* name, const TypeInfo* type, u8* data );
void draw_struct_inspector( const char* name, const TypeInfo* type, u8* data );
void draw_data_inspector( const char* name, const TypeInfo* type, u8* data, bool is_data_pointer );
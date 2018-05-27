#include "store.h"

void cleanup_type_store( TypeStore& store )
{
    if( store.buffer )
        delete[] store.buffer;
    store = {};
}
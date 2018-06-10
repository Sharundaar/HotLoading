#pragma once

#include <array>
#include <iterator>
#include <algorithm>

#include "basic_types.h"
#include "basics.h"

#define DEFAULT_POOL_SIZE 32

class MemoryPoolBase;
template<typename T> class MemoryPoolIterator;
template<typename T> class MemoryPool;

template<typename T>
constexpr u32 get_pool_size() { return DEFAULT_POOL_SIZE; }

namespace
{
    template<typename T>
    struct MemoryBlob
    {
        friend class MemoryPoolIterator<T>;

        std::array<T, get_pool_size<T>()> data;
        std::array<u8, get_pool_size<T>() / 8> allocated;

        u32 high;
        u32 low;

        MemoryBlob<T>* next;

        constexpr u32 size() { return get_pool_size<T>(); }
    };
}

class MemoryPoolBase
{
public:
    virtual void DestroyByIndex( u32 idx ) = 0;
    virtual u32 InstantiateByIndex() = 0;
};
typedef MemoryPoolBase* MemoryPoolPtr;

template<typename T>
class MemoryPool : public MemoryPoolBase
{
    friend class MemoryPoolIterator<T>;

private:
    MemoryBlob<T>* first_blob;
    MemoryBlob<T>* last_blob;

    MemoryBlob<T>* AllocateBlob( MemoryBlob<T>* previous_blob = nullptr )
    {
        auto* new_blob = new MemoryBlob<T>();
        new_blob->next = nullptr;
        new_blob->high = 0;
        new_blob->low  = 0;

        if( previous_blob )
        {
            assert( previous_blob->next == nullptr, "Error: blob chain break.");
            previous_blob->next = new_blob;
            if( previous_blob == last_blob )
                last_blob = new_blob;
        }

        return new_blob;
    }

    // @Optimize: Also return the component index
    T* Instantiate( MemoryBlob<T>* blob )
    {
        u32 inst_idx = (std::min)( blob->low, blob->high );

        u32 allocated_idx  = inst_idx / 8;
        u8 allocated_mask  = 1 << (inst_idx % 8);
        assert( ( blob->allocated[ allocated_idx ] & allocated_mask ) == 0, "Error in the algorithm, should not be allocated." );
        blob->data[inst_idx] = {};
        blob->allocated[ allocated_idx ] |= allocated_mask;

        if( blob->low < blob->high )
        {
            // searching for a block that is not full
            while( blob->allocated[ allocated_idx ] == max_value<u8>() ) allocated_idx++;
            
            // searching for the bit that is unset
            u8 allocated_bit = 0;
            while( ( blob->allocated[allocated_idx] & ( 1 << allocated_bit ) ) > 0 ) allocated_bit++;

            blob->low = allocated_idx * 8 + allocated_bit;
        }
        else
        {
            blob->low++;
            blob->high++;
        }

        return &blob->data[ inst_idx ];
    }

    void Destroy( MemoryBlob<T>* blob, u32 blob_idx )
    {
        u32 allocated_idx = blob_idx / 8;
        u8  allocated_mask = 1 << (blob_idx % 8);
        assert( ( blob->allocated[allocated_idx] & allocated_mask) != 0, "Error: Tried to destroy something not allocated.");

        blob->allocated[ allocated_idx ] &= ~allocated_mask;

        if( blob_idx < blob->low ) blob->low = blob_idx;
    }

    T* Get( MemoryBlob<T>* blob, u32 blob_idx )
    {
        u32 allocated_idx = blob_idx / 8;
        u8  allocated_mask = 1 << (blob_idx % 8);
        assert( ( blob->allocated[allocated_idx] & allocated_mask) != 0, "Error: Tried to get something not allocated.");

        return &blob->data[blob_idx];
    }

public:
    MemoryPool()
    {
        first_blob = AllocateBlob();
        last_blob = first_blob;
    }

    ~MemoryPool()
    {
        auto* blob = first_blob;
        while( blob )
        {
            first_blob = blob->next;
            delete blob;
            blob = first_blob;
        }

        first_blob = nullptr;
        last_blob  = nullptr;
    }

    T* Instantiate()
    {
        MemoryBlob<T>* blob = first_blob;
        while( blob )
        {
            if( blob->high >= blob->size() && blob->low >= blob->high )
            {
                if( blob == last_blob )
                    blob = AllocateBlob( last_blob );
                else
                    blob = blob->next;

                continue;
            }

            return Instantiate( blob );
        }

        assert( false, "Error: Failed to allocate an element." ); // @Note: Should never get here...
        return nullptr;
    }

    void DestroyByIndex( u32 idx ) override
    {
        MemoryBlob<T>* blob = first_blob;
        while( blob && idx >= blob->size() )
        {
            idx -= blob->size();
            blob = blob->next;
        }

        assert( blob != nullptr, "Error: Tried to destroy something that doesn't belong to any blob.");
        Destroy( blob, idx );
    }

    u32 InstantiateByIndex() override
    {
        auto inst = Instantiate();
        return IndexOf( inst );
    }

    void Destroy( T* obj )
    {
        MemoryBlob<T>* blob = first_blob;
        i32 idx = (i32) ( obj - blob->data.data() );
        while( blob && !( idx >= 0 && (u32)idx < blob->size()) )
        {
            blob = blob->next;
            idx = (i32) ( obj - blob->data.data() );
        }
        assert( blob != nullptr, "Error: Tried to destroy something that doesn't belong to any blob.");

        Destroy( blob, idx );
    }

    T* Get( u32 idx )
    {
        MemoryBlob<T>* blob = first_blob;
        while( blob && idx >= blob->size() )
        {
            idx -= blob->size();
            blob = blob->next;
        }

        assert( blob != nullptr, "Error: Tried to get something that doesn't belong to this pool." );
        return Get( blob, idx );
    }

    u32 IndexOf( const T* obj )
    {
        MemoryBlob<T>* blob = first_blob;
        i32 total_idx = 0;
        i32 idx = (i32) ( obj - blob->data.data() );
        while( blob && !( idx >= 0 && idx < (i32)blob->size()) )
        {
            blob = blob->next;
            idx = (i32) ( obj - blob->data.data() );
            total_idx += blob->size();
        }
        assert( blob != nullptr, "Error: Tried to get the index of something that doesn't belong to this pool." );

        total_idx += idx;
        return total_idx;
    }

    MemoryPoolIterator<T> begin()
    {
        return MemoryPoolIterator<T>( *this );
    }

    MemoryPoolIterator<T> end()
    {
        return MemoryPoolIterator<T>( nullptr );
    }
};

template<typename T>
class MemoryPoolIterator
{
private:
    MemoryBlob<T>* current_blob = nullptr;
    u32         current_blob_allocated_idx  = 0;
    u8          current_blob_allocated_bit = 0;

public:
    MemoryPoolIterator( MemoryPool<T>& pool )
        : current_blob( pool.first_blob )
    {
        if( current_blob && !is_valid() )
            ++(*this);
    }

    MemoryPoolIterator( MemoryBlob<T>* blob )
        : current_blob( blob )
    {
        if( current_blob && !is_valid() )
            ++(*this);
    }

    MemoryPoolIterator<T> operator++()
    {
        next();
        while( current_blob && !is_valid() )
            next();

        return *this;
    }

    void next()
    {
        assert( current_blob != nullptr, "Error: Called next with no current_blob." );

        if( current_blob_allocated_bit == 8 )
        {
            current_blob_allocated_bit = 0;
            current_blob_allocated_idx += 1;
        }
        else
        {
            current_blob_allocated_bit += 1;
        }

        if( ( current_blob_allocated_idx * 8 >= current_blob->high ) || ( current_blob_allocated_idx * 8 >= current_blob->size() ) )
        {
            current_blob_allocated_idx = 0;
            current_blob = current_blob->next;
        }
    }

    bool is_valid()
    {
        if( current_blob == nullptr || ( current_blob_allocated_idx * 8 ) > current_blob->size() )
            return false;

        return ( current_blob->allocated[ current_blob_allocated_idx ] & (1 << current_blob_allocated_bit) ) > 0;
    }

    bool operator==( const MemoryPoolIterator<T>& other ) const
    {
        return ( this->current_blob == other.current_blob )
                && ( this->current_blob_allocated_idx == other.current_blob_allocated_idx )
                && ( this->current_blob_allocated_bit == other.current_blob_allocated_bit );
    }

    bool operator!=( const MemoryPoolIterator<T>& other ) const
    {
        return !(*this == other);
    }

    T* operator*() const
    {
        assert( current_blob, "Error: Dereferencing itertor while current_blob is null." );
        return &current_blob->data[ current_blob_allocated_idx * 8 + current_blob_allocated_bit ];
    }

    T* operator->() const
    {
        assert( current_blob, "Error: Dereferencing itertor while current_blob is null." );
        return &current_blob->data[ current_blob_allocated_idx * 8 + current_blob_allocated_bit ];
    }
};

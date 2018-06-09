#pragma once
#include <ym_core.h>

// Taken from & Based on: http://ourmachinery.com/post/minimalist-container-library-in-c-part-1/

typedef struct
{
    u32 size;
    u32 capacity;

    // Should also add allocator?
} ym_array_header;

// Find out how to deal with it having an allocation and deallocation strategy

#define ym_array_get_header(array) \
    ((ym_array_header*)((char*)(array) - sizeof(ym_array_header)))

#define ym_array_size(array) \
    ((array) ? ym_array_get_header(array)->size : 0)

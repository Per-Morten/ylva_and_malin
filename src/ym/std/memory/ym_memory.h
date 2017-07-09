#pragma once
#include <heap_allocator/ym_heap_allocator.h>
#include <stack_frame_allocator/ym_stack_frame_allocator.h>
#include <algorithm/ym_construct.h>
#include <algorithm/ym_destruct.h>

namespace ym::memory_globals
{
    static inline heap_allocator default_allocator;
}

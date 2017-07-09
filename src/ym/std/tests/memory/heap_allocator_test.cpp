#include <catch.h>
#include <ym_memory.h>

SCENARIO("Heap allocators can allocate and deallocate aligned memory",
         "[heap_allocator aligned]")
{
    GIVEN("Empty allocator")
    {
        ym::heap_allocator allocator;
        REQUIRE(allocator.size() == 0);
        void* ptr = nullptr;

        WHEN("an allocation is requested")
        {
            ptr = allocator.allocate_aligned(sizeof(int), alignof(int));

            THEN("memory is returned and the size is increased")
            {
                REQUIRE(allocator.size() == sizeof(int) + alignof(int));
                REQUIRE(ptr != nullptr);
            }
        }

        allocator.deallocate_aligned(ptr, sizeof(int), alignof(int));
    }

    GIVEN("A used allocator")
    {
        ym::heap_allocator allocator;
        REQUIRE(allocator.size() == 0);
        void* ptr = allocator.allocate_aligned(sizeof(int), alignof(int));
        REQUIRE(allocator.size() == sizeof(int) + alignof(int));

        WHEN("A deallocation is requested")
        {
            allocator.deallocate_aligned(ptr, sizeof(int), alignof(int));

            THEN("memory is deallocated and size is decreased")
            {
                REQUIRE(allocator.size() == 0);
            }
        }
    }

    GIVEN("Empty allocator")
    {
        ym::heap_allocator allocator;
        REQUIRE(allocator.size() == 0);

        WHEN("an allocation for an array is requested")
        {
            auto ptr
                = static_cast<int*>(allocator.allocate_aligned(sizeof(int) * 4, alignof(int)));

            THEN("memory is returned and the size is increased, and the memory can be used")
            {
                for (ym::ptrdiff_t i = 0; i < 4; i++)
                    ptr[i] = i;

                REQUIRE(allocator.size() == sizeof(int) * 4 + alignof(int));
                REQUIRE([=]{ return ptr[0] == 0 && ptr[1] == 1 && ptr[2] == 2 && ptr[3] == 3; }());
            }
            allocator.deallocate_aligned(ptr, sizeof(int) * 4, alignof(int));
            REQUIRE(allocator.size() == 0);
        }
    }
}

SCENARIO("Heap allocators can allocate and deallocate unaligned memory",
         "[heap_allocator unaligned]")
{
    GIVEN("Empty allocator")
    {
        ym::heap_allocator allocator;
        REQUIRE(allocator.size() == 0);
        void* ptr = nullptr;

        WHEN("an allocation is requested")
        {
            ptr = allocator.allocate(sizeof(int));
            THEN("memory is returned and the size is increased")
            {
                REQUIRE(allocator.size() == sizeof(int));
                REQUIRE(ptr != nullptr);
            }
        }

        allocator.deallocate(ptr, sizeof(int));
    }

    GIVEN("A used allocator")
    {
        ym::heap_allocator allocator;
        REQUIRE(allocator.size() == 0);
        void* ptr = allocator.allocate(sizeof(int));
        REQUIRE(allocator.size() == sizeof(int));

        WHEN("A deallocation is requested")
        {
            allocator.deallocate(ptr, sizeof(int));

            THEN("memory is deallocated and size is decreased")
            {
                REQUIRE(allocator.size() == 0);
            }
        }
    }

    GIVEN("Empty allocator")
    {
        ym::heap_allocator allocator;
        REQUIRE(allocator.size() == 0);

        WHEN("an allocation for an array is requested")
        {
            auto ptr
                = static_cast<int*>(allocator.allocate(sizeof(int) * 4));

            THEN("memory is returned and the size is increased, and the memory can be used")
            {
                for (ym::ptrdiff_t i = 0; i < 4; i++)
                    ptr[i] = i;

                REQUIRE(allocator.size() == sizeof(int) * 4);
                REQUIRE([=]{ return ptr[0] == 0 && ptr[1] == 1 && ptr[2] == 2 && ptr[3] == 3; }());
            }
            allocator.deallocate(ptr, sizeof(int) * 4);
            REQUIRE(allocator.size() == 0);
        }
    }
}

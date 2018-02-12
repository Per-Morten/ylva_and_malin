#include <ym_test.h>
#include <ym_core.h>
#include <ym_allocator.h>
#include <stdbool.h>

typedef const char* (*test_func)();
u8 g_total_memory[4096];

ym_allocator
get_region_allocator()
{
    ym_allocator allocator;
    allocator.id = 1;

//    u32 number = 128 | 256;
//    for (int i = 31; i >= 0; i--)
//        printf("%c", number & (1 << i) ? '1' : '0');
//    printf("\n");

    ym_create_allocator(ym_alloc_strategy_region,
                        g_total_memory,
                        4096,
                        &(ym_allocator_cfg)
                        {
                            .slot_size = (int[]){16, 32, 128, 1024},
                            .slot_count = (int[]){4, 4, 4, 1},
                            .count = 4,
                        },
                        &allocator);

    return allocator;
}

bool
region_sanity_check(void** valid_stack,
                    const int stack_count,
                    uintptr_t* start)
{
    bool sanity_check = true;
    uintptr_t* itr = start;
    int i = 0;
    while (itr && i < stack_count)
    {
        if (itr != valid_stack[i])
        {
            YM_WARN("itr not valid: i: %d, itr: %p valid: %p", i, itr, valid_stack[i]);
            sanity_check = false;
        }
        itr = (uintptr_t*)*itr;
        i++;
    }

    return sanity_check;
}

uintptr_t*
get_heads(ym_allocator* allocator)
{
    return allocator->free_list.heads;
}

const char*
test_region_same_order_allocate_deallocate()
{
    ym_allocator allocator = get_region_allocator();

    uintptr_t* heads = get_heads(&allocator);

    void* mock_ptrs[4] = {NULL};
    const int mock_ptrs_count = sizeof(mock_ptrs) / sizeof(mock_ptrs[0]);

    for (int i = 0; i < mock_ptrs_count; i++)
    {
        ym_errc errc = ym_allocate(&allocator, 40, &mock_ptrs[i]);
        YM_CHECK("errc is success", errc == ym_errc_success);
        YM_CHECK("memory isn't NULL", mock_ptrs[i]);
    }

    YM_CHECK("head is NULL, as we don't have anymore space",
             heads[2] == (uintptr_t)NULL);

    for (int i = 0; i < mock_ptrs_count; i++)
    {
        ym_errc errc = ym_deallocate(&allocator, 40, mock_ptrs[i]);
        YM_CHECK("errc is success", errc == ym_errc_success);
        YM_CHECK("head is newly deallocated", heads[2] == (uintptr_t)mock_ptrs[i]);
    }

    void* correct_stack[mock_ptrs_count];
    for (int i = 0; i < mock_ptrs_count; i++)
        correct_stack[i] = mock_ptrs[mock_ptrs_count - 1 - i];

    bool sanity_check = region_sanity_check((void**)&correct_stack, mock_ptrs_count, (uintptr_t*)heads[2]);
    YM_CHECK("sanity check failed, the stack doesn't function correctly",
             sanity_check);

    return NULL;
}

const char*
test_region_random_order_allocate_deallocate()
{
    ym_allocator allocator = get_region_allocator();

    uintptr_t* heads = get_heads(&allocator);

    void* mock_ptrs[4] = {NULL};
    const int mock_ptrs_count = sizeof(mock_ptrs) / sizeof(mock_ptrs[0]);

    // Obviously this is random :P
    int random_table[4] = {1, 0, 3, 2};

    for (int i = 0; i < mock_ptrs_count; i++)
    {
        ym_errc errc = ym_allocate(&allocator, 40, &mock_ptrs[random_table[i]]);
        YM_CHECK("errc is success", errc == ym_errc_success);
        YM_CHECK("memory isn't NULL", mock_ptrs[random_table[i]]);
    }

    YM_CHECK("head is NULL, as we don't have anymore space",
             heads[2] == (uintptr_t)NULL);

    for (int i = 0; i < mock_ptrs_count; i++)
    {
        ym_errc errc = ym_deallocate(&allocator, 40, mock_ptrs[random_table[i]]);
        YM_CHECK("errc is success", errc == ym_errc_success);
        YM_CHECK("head is newly deallocated", heads[2] == (uintptr_t)mock_ptrs[random_table[i]]);
    }

    void* correct_stack[mock_ptrs_count];
    for (int i = 0; i < mock_ptrs_count; i++)
        correct_stack[i] = mock_ptrs[random_table[mock_ptrs_count - 1 - i]];

    bool sanity_check = region_sanity_check((void**)&correct_stack, mock_ptrs_count, (uintptr_t*)heads[2]);
    YM_CHECK("sanity check failed, the stack doesn't function correctly",
             sanity_check);

    return NULL;
}

const char*
test_region_random_memory_reuse()
{
    ym_allocator allocator = get_region_allocator();
    uintptr_t* heads = get_heads(&allocator);

    void* mock_ptrs[4] = {NULL};
    const int mock_ptrs_count = sizeof(mock_ptrs) / sizeof(mock_ptrs[0]);

    for (int i = 0; i < mock_ptrs_count; i++)
    {
        ym_errc errc = ym_allocate(&allocator, 40, &mock_ptrs[i]);
        YM_CHECK("errc is success", errc == ym_errc_success);
        YM_CHECK("memory isn't NULL", mock_ptrs[i]);
    }

    YM_CHECK("head is NULL, as we don't have anymore space",
             heads[2] == (uintptr_t)NULL);

    for (int i = 0; i < 2; i++)
    {
        ym_errc errc = ym_deallocate(&allocator, 40, mock_ptrs[i]);
        YM_CHECK("errc is success", errc == ym_errc_success);
        YM_CHECK("head is newly deallocated", heads[2] == (uintptr_t)mock_ptrs[i]);
    }

    void* new_ptrs[2];
    for (int i = 0; i < 2; i++)
    {
        ym_errc errc = ym_allocate(&allocator, 40, &new_ptrs[i]);
        YM_CHECK("errc is success", errc == ym_errc_success);
        YM_CHECK("memory is reused", new_ptrs[i] = mock_ptrs[2 - i]);
    }

    for (int i = 2; i < 4; i++)
    {
        ym_errc errc = ym_deallocate(&allocator, 40, mock_ptrs[i]);
        YM_CHECK("errc is success", errc == ym_errc_success);
        YM_CHECK("head is newly deallocated", heads[2] == (uintptr_t)mock_ptrs[i]);
    }

    for (int i = 0; i < 2; i++)
    {
        ym_errc errc = ym_deallocate(&allocator, 40, new_ptrs[i]);
        YM_CHECK("errc is success", errc == ym_errc_success);
        YM_CHECK("head is newly deallocated", heads[2] == (uintptr_t)new_ptrs[i]);
    }

    void* correct_stack[4] =
    {
        new_ptrs[1],
        new_ptrs[0],
        mock_ptrs[3],
        mock_ptrs[2],
    };
    bool sanity_check = region_sanity_check((void**)&correct_stack, 4, (uintptr_t*)heads[2]);
    YM_CHECK("sanity check failed, the stack doesn't function correctly",
             sanity_check);

    return NULL;
}

const char*
test_region_errc_on_null()
{
    ym_allocator allocator = get_region_allocator();
    ym_errc errc = ym_deallocate(&allocator, 40, NULL);
    YM_CHECK("errc should not be success", errc != ym_errc_success);

    return NULL;
}

const char*
test_errc_on_out_of_memory()
{
    // TODO: What should happen here? Cause technically I want to fallback on malloc initially.

    return NULL;
}

const char*
test_errc_on_invalid_dealloc_request()
{
    // TODO: This should deal with the case that the memory doesn't actually come from the allocator

    return NULL;
}

const char*
test_errc_on_invalid_alloc_size_request()
{
    void* ptr;
    ym_allocator allocator = get_region_allocator();
    ym_errc errc = ym_allocate(&allocator, 4096, &ptr);
    YM_CHECK("errc should not be success", errc != ym_errc_success);

    return NULL;
}

const char*
test_errc_on_invalid_dealloc_size_request()
{
    // TODO: This should deal with the case that the memory doesn't actually come from the allocator

    return NULL;
}

int
main(YM_UNUSED int argc,
     YM_UNUSED char** argv)
{
    test_func tests[] =
    {
        test_region_same_order_allocate_deallocate,
        test_region_random_order_allocate_deallocate,
        test_region_random_memory_reuse,
        test_region_errc_on_null,
        test_errc_on_out_of_memory,
        test_errc_on_invalid_dealloc_request,
        test_errc_on_invalid_alloc_size_request,
        //test_errc_on_invalid_dealloc_size_request,
    };

    const char* test_names[] =
    {
        "test_region_same_order_allocate_deallocate",
        "test_region_random_order_allocate_deallocate",
        "test_region_random_memory_reuse",
        "test_region_errc_on_null",
        "test_errc_on_out_of_memory",
        "test_errc_on_invalid_dealloc_request",
        "test_errc_on_invalid_alloc_size_request",
        //"test_errc_on_invalid_dealloc_size_request",
    };

    int passed_tests = 0;

    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        const char* message = tests[i]();
        if (message)
        {
            YM_INFO("%sFailed:%s %s, msg: %s",
                    YM_COLOR_FG_RED, YM_COLOR_RESET,
                    test_names[i], message);
        }
        else
        {
            YM_INFO("%sSuccess:%s %s", YM_COLOR_FG_GREEN, YM_COLOR_RESET, test_names[i]);
            passed_tests++;
        }
    }

    YM_INFO("Passed: %d of %d tests",
            passed_tests,
            sizeof(tests) / sizeof(tests[0]));


    return passed_tests != 0;
}

#define CATCH_CONFIG_RUNNER
#include <catch.h>
#include <ym_memory.h>

TEST_CASE("Memory can be allocated and deallocated", "[memory]")
{
    ym_errc errc = ym_mem_init();
    REQUIRE(errc == ym_errc_success);

    ym_mem_region mem;

    SECTION("User wants access to region")
    {
//        mem = ym_mem_alloc(ym_mem_reg_dbg_str, 4);
//
//        REQUIRE(mem.mem != NULL);
//        REQUIRE(ym_clear_error() == ym_errc_success);
    }

    SECTION("User wants to deallocate back to a given region")
    {
//        ym_mem_dealloc(&mem);
//        REQUIRE(ym_clear_error() == ym_errc_success);
    }

    SECTION("User tries to allocate from invalid region")
    {
//        mem = ym_mem_alloc(ym_mem_reg_count, 10);
//
//        REQUIRE(ym_clear_error() == ym_errc_invalid_input);
    }

    errc = ym_mem_shutdown();
    REQUIRE(errc == ym_errc_success);
}

int
main(int argc, char** argv)
{
    int result = Catch::Session().run(argc, argv);

    return (result < 0xff ? result : 0xff);
}

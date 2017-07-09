#define CATCH_CONFIG_RUNNER
#include <catch.h>
#include <ym_log.h>

int
main(int argc, char** argv)
{
    int result = Catch::Session().run(argc, argv);

    YM_WARN("Warning test %zu", ym::size_t(1));
    YM_INFO("Info test");
    YM_DEBUG("Debug test");
    return (result < 0xff ? result : 0xff);
}

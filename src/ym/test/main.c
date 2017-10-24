#include <ym_test.h>
#include <stdio.h>

int tests_run = 0;

const char*
test_0()
{
    int var = 1;
    YM_CHECK("var != 1", var == 1);

    return NULL;
}

const char*
test_1()
{
    int var = 5;
    YM_CHECK("var != 1", var == 1);

    return NULL;
}

const char*
run_all_tests()
{
    YM_TEST(test_0);
    YM_TEST(test_1);
    return NULL;
}

int
main(int argc, char** argv)
{
    const char* result = run_all_tests();

    if (result)
    {
        printf("%s\n", result);
    }
    else
    {
        printf("All tests passed\n");
    }
    printf("Tests run: %d\n", tests_run);

    return 0;
}

#pragma once

///////////////////////////////////////////////////////////
// Taken directly from:
//  http://www.jera.com/techinfo/jtns/jtn002.html
//
// Just changed some names, and minor formatting.
///////////////////////////////////////////////////////////

#define YM_CHECK(message, test) \
do                              \
{                               \
    if (!(test))                \
        return "\033[0;31m" message "\033[0m"; \
} while (0)


#define YM_TEST(test)                 \
do                                    \
{                                     \
    const char* message = test();     \
    tests_run++;                      \
    if (message)                      \
        return message;               \
} while (0)

 extern int tests_run;

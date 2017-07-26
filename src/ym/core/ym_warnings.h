#pragma once
YM_EXTERN_CPP_BEGIN

#define YM_PUSH_WARNING_TERMINATE   \
_Pragma("GCC diagnostic push")      \
_Pragma("GCC diagnostic ignored \"-Wterminate\"")

#define YM_POP_WARNING          \
_Pragma("GCC diagnostic pop")

YM_EXTERN_CPP_END

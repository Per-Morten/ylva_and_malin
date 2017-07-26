#pragma once

#ifdef __cplusplus
#define YM_EXTERN_CPP_BEGIN   \
extern "C"                    \
{
#else
#define YM_EXTERN_CPP_BEGIN
#endif

#ifdef __cplusplus
#define YM_EXTERN_CPP_END     \
}
#else
#define YM_EXTERN_CPP_END
#endif

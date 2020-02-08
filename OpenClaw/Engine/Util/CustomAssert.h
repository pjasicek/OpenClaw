#ifndef OPENCLAW_CUSTOMASSERTS_H
#define OPENCLAW_CUSTOMASSERTS_H

#include <assert.h>

//#ifdef __FUNCTION__
//#define __FUNC_NAME__ __FUNCTION__
//#endif
//#ifdef __func__
//#define __FUNC_NAME__ __func__
//#endif
//#ifndef __FUNC_NAME__
//#define __FUNC_NAME__ ""
//#endif

#ifndef DO_AND_CHECK
#define DO_AND_CHECK(cmd) \
do { \
    if (!(cmd)) { \
        LOG_ERROR(ToStr("Check error: ") + #cmd); \
        std::abort(); \
    } \
} while(0);
#endif

#ifndef cond_assert
#define cond_assert(enabled, body) { if (enabled) { DO_AND_CHECK(body); } else { body; } }
#endif

#endif //OPENCLAW_CUSTOMASSERTS_H

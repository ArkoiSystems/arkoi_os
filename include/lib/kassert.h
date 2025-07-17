#ifndef ASSERT_H
#define ASSERT_H

#include "kpanic.h"

#ifdef NDEBUG
    #define ASSERT(expression) ((void) 0)
#else
    #define ASSERT_M(expression, message)                                 \
        do {                                                              \
            if (!(expression)) {                                          \
                PANIC("Assertion failed: " #expression " (%s)", message); \
            }                                                             \
        } while (0);

    #define ASSERT(expression)                           \
        do {                                             \
            if (!(expression)) {                         \
                PANIC("Assertion failed: " #expression); \
            }                                            \
        } while (0);
#endif


#endif //ASSERT_H

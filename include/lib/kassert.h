#ifndef KASSERT_H
#define KASSERT_H

#include "kpanic.h"

// This is a compile-time assertion. If the condition is false, the compiler will generate an error due to the negative
// array size.
#define STATIC_ASSERT(cond, msg) typedef char STATIC_ASSERTION_##msg[(cond) ? 1 : -1]

#ifdef NDEBUG
#define KASSERT(expression) (() 0)
#else
#define KASSERT_M(expression, message)                                                                                 \
    do {                                                                                                               \
        if (!(expression)) {                                                                                           \
            KPANIC("Assertion failed: " #expression " (%s)", message);                                                 \
        }                                                                                                              \
    } while (0);

#define KASSERT(expression)                                                                                            \
    do {                                                                                                               \
        if (!(expression)) {                                                                                           \
            KPANIC("Assertion failed: " #expression);                                                                  \
        }                                                                                                              \
    } while (0);
#endif

#endif // KASSERT_H

#ifndef KASSERT_H
#define KASSERT_H

#include "kpanic.h"

#ifdef NDEBUG
    #define KASSERT(expression) ((void) 0)
#else
    #define KASSERT_M(expression, message)                                 \
        do {                                                              \
            if (!(expression)) {                                          \
                KPANIC("Assertion failed: " #expression " (%s)", message); \
            }                                                             \
        } while (0);

    #define KASSERT(expression)                           \
        do {                                             \
            if (!(expression)) {                         \
                KPANIC("Assertion failed: " #expression); \
            }                                            \
        } while (0);
#endif


#endif //KASSERT_H

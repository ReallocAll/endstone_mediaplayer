#ifndef CPPCOMPAT_COMMON_H
#define CPPCOMPAT_COMMON_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * Internal utilities shared across cppcompat modules.
 * Not part of the public API.
 */

static inline void *cpp_alloc(size_t size) {
    return malloc(size);
}

static inline void cpp_dealloc(void *ptr) {
    free(ptr);
}

#endif /* CPPCOMPAT_COMMON_H */

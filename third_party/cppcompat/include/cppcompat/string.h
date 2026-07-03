#ifndef CPPCOMPAT_STRING_H
#define CPPCOMPAT_STRING_H

#include <stddef.h>
#include <stdbool.h>
#include "abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Pure C construction of MSVC-compatible std::string (32 bytes, x64).
 *
 * cppcompat only handles object lifecycle (construct/destroy/copy/move).
 * All std::string member functions are executed by the real MSVC STL
 * after reinterpret_cast to std::string*.
 */

/* In-place construction at dst. dst must be CPP_STRING_SIZE bytes. */
void cpp_string_construct(void *dst, const char *str);

/* Destroy: free internal heap buffer. Does NOT free dst itself. */
void cpp_string_destroy(void *s);

/* Heap-allocate + construct. Caller owns the returned pointer. */
void *cpp_string_new(const char *str);

/* Destroy + free the 32-byte object. Pass NULL is safe. */
void cpp_string_delete(void *s);

/* Copy: construct at dst from src (both are cppcompat string objects). */
void cpp_string_copy(void *dst, const void *src);

/* Move: transfer ownership from src to dst. src becomes empty. */
void cpp_string_move(void *dst, void *src);

/* Get C string pointer. Valid while the object lives. */
const char *cpp_string_str(const void *s);

#ifdef __cplusplus
}
#endif

#endif /* CPPCOMPAT_STRING_H */

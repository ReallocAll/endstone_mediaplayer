#ifndef CPPCOMPAT_VECTOR_H
#define CPPCOMPAT_VECTOR_H

#include <stddef.h>
#include <stdbool.h>
#include "abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Pure C construction of MSVC-compatible std::vector<T> (24 bytes, x64).
 *
 * Layout: { void *_Myfirst; void *_Mylast; void *_Myend; }
 *
 * cppcompat only handles object lifecycle (construct/destroy).
 * All std::vector member functions are executed by the real MSVC STL
 * after reinterpret_cast to std::vector<T>*.
 *
 * elem_destroy: optional callback to destroy each element (called during
 *               cpp_vector_destroy for non-trivial types like std::string).
 *               Pass NULL for trivial types (int, float, POD).
 */

/* In-place construction of empty vector. */
void cpp_vector_construct(void *dst, size_t elem_size,
                          void (*elem_destroy)(void *elem));

/* In-place construction with initial data. data may be NULL for empty. */
void cpp_vector_construct_with(void *dst, size_t elem_size,
                               const void *data, size_t count,
                               void (*elem_destroy)(void *elem));

/* Destroy: call elem_destroy on each element, free internal buffer. */
void cpp_vector_destroy(void *v);

/* Heap-allocate + construct empty vector. */
void *cpp_vector_new(size_t elem_size,
                     void (*elem_destroy)(void *elem));

/* Destroy + free the 24-byte object. */
void cpp_vector_delete(void *v);

/* Get pointer to internal buffer (_Myfirst). NULL if empty. */
void *cpp_vector_data(const void *v);

/* Get element count (_Mylast - _Myfirst) / elem_size. */
size_t cpp_vector_size(const void *v, size_t elem_size);

#ifdef __cplusplus
}
#endif

#endif /* CPPCOMPAT_VECTOR_H */

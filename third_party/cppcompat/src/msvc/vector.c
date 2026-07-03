#include "cppcompat/vector.h"
#include "common/common.h"

/**
 * MSVC std::vector<T> ABI implementation.
 *
 * Memory layout (x64, 24 bytes):
 *   [0-7]   void *_Myfirst  (pointer to buffer start)
 *   [8-15]  void *_Mylast   (pointer past last element)
 *   [16-23] void *_Myend    (pointer past allocated end)
 *
 * Internal buffer allocated with malloc to match MSVC's allocator.
 */

static void vector_set_ptrs(void *dst, void *first, void *last, void *end)
{
    void **ptrs = (void **)dst;
    ptrs[0] = first;
    ptrs[1] = last;
    ptrs[2] = end;
}

void cpp_vector_construct(void *dst, size_t elem_size,
                          void (*elem_destroy)(void *elem))
{
    (void)elem_size;
    (void)elem_destroy;
    if (!dst) return;
    vector_set_ptrs(dst, NULL, NULL, NULL);
}

void cpp_vector_construct_with(void *dst, size_t elem_size,
                               const void *data, size_t count,
                               void (*elem_destroy)(void *elem))
{
    (void)elem_destroy;
    if (!dst) return;

    if (!data || count == 0 || elem_size == 0) {
        vector_set_ptrs(dst, NULL, NULL, NULL);
        return;
    }

    size_t total = elem_size * count;
    void *buf = cpp_alloc(total);
    if (!buf) {
        vector_set_ptrs(dst, NULL, NULL, NULL);
        return;
    }

    memcpy(buf, data, total);

    char *first = (char *)buf;
    char *last = first + total;
    vector_set_ptrs(dst, first, last, last);
}

void cpp_vector_destroy(void *v)
{
    if (!v) return;
    void **ptrs = (void **)v;
    void *first = ptrs[0];
    if (first) {
        cpp_dealloc(first);
    }
    vector_set_ptrs(v, NULL, NULL, NULL);
}

void *cpp_vector_new(size_t elem_size,
                     void (*elem_destroy)(void *elem))
{
    void *obj = cpp_alloc(CPP_VECTOR_SIZE);
    if (!obj) return NULL;
    cpp_vector_construct(obj, elem_size, elem_destroy);
    return obj;
}

void cpp_vector_delete(void *v)
{
    if (!v) return;
    cpp_vector_destroy(v);
    cpp_dealloc(v);
}

void *cpp_vector_data(const void *v)
{
    if (!v) return NULL;
    void *const *ptrs = (void *const *)v;
    return ptrs[0];
}

size_t cpp_vector_size(const void *v, size_t elem_size)
{
    if (!v || elem_size == 0) return 0;
    char *const *ptrs = (char *const *)v;
    return (size_t)(ptrs[1] - ptrs[0]) / elem_size;
}

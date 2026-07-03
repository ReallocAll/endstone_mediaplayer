/**
 * sfunc.c — std::function ABI construction for pure C.
 *
 * Provides func_impl_t allocation and std::function buffer building
 * for both std::function<void(Event&)> and std::function<void()>.
 * Used by plugin.c for event and scheduler registration.
 */

#include "abi_helpers.h"
#include <string.h>

/* ── Shared _Func_impl vtable slots ───────────────────────────────── */

static void *sfunc_copy(void *self, void *dst)
{
    memcpy(dst, self, 24);
    return dst;
}

static void *sfunc_target_type(void *self) { (void)self; return nullptr; }
static void *sfunc_get(void *self)         { (void)self; return nullptr; }
static void  sfunc_delete_this(void *self, int d) { (void)self; (void)d; }

/* ── std::function<void(Event&)> ──────────────────────────────────── */

static void sf_evt_trampoline(void *instance, void *event)
{
    (void)instance;
    ((void (*)(void *))instance)(event);
}

static void sf_evt_do_call(void *self, void *event)
{
    void (*fn)(void *, void *) = *(void (**)(void *, void *))((char *)self + 8);
    void *inst = *(void **)((char *)self + 16);
    if (fn) fn(inst, event);
}

/* ── std::function<void()> ────────────────────────────────────────── */

static void sf_void_trampoline(void *instance)
{
    ((void (*)(void))instance)();
}

static void sf_void_do_call(void *self)
{
    void (*fn)(void *) = *(void (**)(void *))((char *)self + 8);
    void *inst = *(void **)((char *)self + 16);
    if (fn) fn(inst);
}

/* ── Vtable arrays ────────────────────────────────────────────────── */

static void *g_sfunc_evt_vt[SFUNC_SLOTS] = {
    sfunc_copy, sfunc_copy, sf_evt_do_call,
    sfunc_target_type, sfunc_delete_this, sfunc_get,
};
static void *g_sfunc_void_vt[SFUNC_SLOTS] = {
    sfunc_copy, sfunc_copy, sf_void_do_call,
    sfunc_target_type, sfunc_delete_this, sfunc_get,
};

/* ── Pool ─────────────────────────────────────────────────────────── */

#define SFUNC_POOL_SIZE 8
static func_impl_t g_sfunc_pool[SFUNC_POOL_SIZE];
static int g_sfunc_count = 0;

func_impl_t *sfunc_alloc(void *handler, bool is_void)
{
    if (g_sfunc_count >= SFUNC_POOL_SIZE) return nullptr;
    func_impl_t *impl = &g_sfunc_pool[g_sfunc_count++];
    impl->vptr     = is_void ? g_sfunc_void_vt : g_sfunc_evt_vt;
    impl->func     = is_void ? (void *)sf_void_trampoline : (void *)sf_evt_trampoline;
    impl->instance = handler;
    return impl;
}
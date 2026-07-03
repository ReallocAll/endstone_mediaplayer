#ifndef CPPCOMPAT_ABI_H
#define CPPCOMPAT_ABI_H

#include <stddef.h>

/**
 * MSVC STL ABI constants for x86_64 Release builds.
 *
 * All values derived from sizeof probe and MSVC STL source analysis.
 * Do NOT guess — verify with /d1reportSingleClassLayout or runtime probe.
 */

/* std::string — 32 bytes, SSO threshold 15 chars */
#define CPP_STRING_SIZE       32
#define CPP_STRING_SSO_CAP    15
#define CPP_STRING_MAX        4095

/* std::vector<T> — 24 bytes (3 pointers: _Myfirst, _Mylast, _Myend) */
#define CPP_VECTOR_SIZE       24

/* std::filesystem::path — 32 bytes, internally std::wstring */
/* Same layout as std::string but with wchar_t (2 bytes per char on Windows) */
#define CPP_PATH_SIZE         32
#define CPP_PATH_SSO_CAP      15   /* 15 wchar_t units */

#endif /* CPPCOMPAT_ABI_H */

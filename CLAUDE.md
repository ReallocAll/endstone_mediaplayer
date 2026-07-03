# CLAUDE.md — endstone_mediaplayer

## Project

Pure C23 Endstone plugin for NBS music playback in Minecraft Bedrock Dedicated Server.
Manually constructs MSVC C++ ABI objects (vtable, std::string, std::function) to
interface with the Endstone C++ framework without a C++ compiler.

## Language & Toolchain

- **C standard**: C23 (`-std:c23`).  Backward compatibility with C11/C17/C99 is not required.
- **Primary compiler**: Clang 22 (`clang-cl` on Windows, MSVC-compatible command line).
- **Build system**: CMake 3.29+ with Ninja generator.
- **MSVC compatibility**: Only required for ABI interop (STL object layout).  Do not use
  MSVC-specific extensions unless explicitly needed for ABI correctness.
- **Target**: Windows x64, MSVC ABI (`x86_64-pc-windows-msvc`).

## Coding Principles

- Prefer **standard C23** features when they clearly improve readability, safety,
  maintainability, or correctness.
- Do not introduce modern language features solely because they are available.
- Favor **simple and idiomatic C** over clever or overly abstract code.
- Avoid unnecessary metaprogramming, macro tricks, or compiler extensions.
- Prefer standard C23 over compiler-specific extensions whenever practical.
- Introduce compiler-specific features only when they provide a significant
  engineering benefit (e.g., `__declspec(dllexport)` for the DLL entry point).
- Use C23 features conservatively and intentionally.
- Do not rewrite existing working code merely to use newer language features.
- Keep APIs and implementations easy to understand for experienced C developers.
- **Prioritize consistency with the surrounding codebase** over adopting the newest syntax.

## Code Style

- Use `//` comments, not `/* */`.
- No `typedef` — all structs referenced as `struct tag`.
- Functions: `snake_case`.  Types: `snake_case` with `_t` suffix.
- Constants: `UPPER_SNAKE_CASE`.  Vtable slots: `ES_CLASS_SLOT_METHOD`.
- Offsets: `ES_CLASS_OFF_FIELD`.  Globals: `g_` prefix.
- Use `stdint.h` types (`uint64_t`, `int32_t`) for ABI-critical code.
- Use `static` for all internal functions and variables.
- `calloc` for heap allocations (zero-initialized).  Check for Nnullptr after allocation.
- Use `nullptr` (C23 keyword), never `NULL`.
- Use standard include guards (`#ifndef`/`#define`/`#endif`), never `#pragma once`.

## Memory Management

- Use `cppcompat` library for all MSVC STL object construction/destruction.
- `cppcompat` uses `malloc`/`free`; Endstone uses `operator new`/`delete`.
  These are **different allocators** — never call `free()` on a pointer that
  Endstone allocated, and never let Endstone `delete` a `malloc`'d pointer.
- For `std::string` pass-by-value to C++ APIs: use `STR_GUARD` macro from
  `abi_helpers.h`.  The C++ callee calls `operator delete` on the heap buffer
  for heap-allocated strings (>15 chars).  `STR_GUARD` saves the pre-call state
  and either destroys normally (SSO) or resets to SSO-empty (heap) to avoid
  double-free.
- `createBossBar` uses `std::move(title)` — the string is moved, not destroyed.
  `cpp_string_destroy` is safe after this call.
- Zero vectors in `plugin_on_disable` to prevent framework cleanup crashes.

## Key ABI Details

- `std::string`: 32 bytes, SSO threshold 15 chars, `_Myres` at offset 24.
- `std::vector`: 24 bytes (three pointers: `_Myfirst`, `_Mylast`, `_Myend`).
- `std::function`: 64 bytes, `_Func_impl` self-pointer at offsets 0x30 and 0x38.
- `std::variant<...>`: 64 bytes, index at offset 32.
- `unique_ptr`: 8 bytes, returned via hidden pointer (non-trivial destructor).
- `shared_ptr`: 16 bytes (pointer + control block).
- Microsoft x64 calling convention: RCX/RDX/R8/R9 + stack.  Hidden pointer for
  return values > 8 bytes and pass-by-value parameters > 8 bytes.

## Verified Vtable Slots

Source: cpp-example-plugin IDA decompilation (vtable offset / 8 = slot).

| Slot | Class | Method |
|------|-------|--------|
| 38 | Server | `createBossBar(string, BarColor, BarStyle)` |
| 14 | BossBar | `addPlayer(Player&)` |
| 13 | BossBar | `setVisible(bool)` |
| 11 | BossBar | `setProgress(float)` |
| 2 | BossBar | `setTitle(string)` |
| 0 | BossBar | destructor |
| 65 | Player | `playSound(Location, string, float, float)` |
| 86 | Player | `sendPopup(string)` |
| 24 | Actor→Player | `getLocation()` |

## Project Structure

```
include/   — Public headers (abi_helpers.h, endstone_abi.h, music_player.h)
src/       — Source
  plugin.c        — Plugin lifecycle, command handler, event/scheduler registration
  music_player.c  — Music playback engine (cache, playlist, tick)
  endstone_api.c  — Endstone API wrappers (Player, BossBar, sendMessage, fopen_utf8)
  sfunc.c         — std::function ABI construction (vtable, trampoline, pool)
third_party/ — External libraries (cppcompat, nbsparser, stb)
  cppcompat/      — MSVC STL ABI compatibility (compiled directly, no .lib)
  nbsparser/      — NBS file format parser
  stb/            — stb_ds dynamic array (MIT)
build/     — Build output (compile_commands.json generated here)
ref/       — IDA decompilation reference (gitignored, do not modify)
```

## Building

```bash
cmake -G Ninja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=RelWithDebInfo -B build
cmake --build build
```

Output: `build/endstone_mediaplayer.dll`

## Testing

Deploy DLL to `C:/Users/ReallocAll/Code/cpp/endstone/bedrock_server/plugins/`,
then start the server and use `/mpm help` for commands.

## Git Conventions

Follow the same format as [Endstone](https://github.com/EndstoneMC/endstone):

```
type: description
```

Types: `feat`, `fix`, `refactor`, `build`, `ci`, `docs`, `chore`, `Release`.

- All lowercase, no period at end.
- Description is imperative ("add feature" not "added feature").
- Keep the first line ≤72 characters.
cmake_minimum_required(VERSION 3.29)

set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)

project(endstone_mediaplayer LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Generate compile_commands.json for clangd/LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Shared CRT (must match Endstone)
if(MSVC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MD" CACHE STRING "" FORCE)
endif()

# Third-party source files (compiled directly, no separate .lib)
set(THIRD_PARTY_SOURCES
    third_party/cppcompat/src/msvc/string.c
    third_party/cppcompat/src/msvc/vector.c
    third_party/nbsparser/nbsparser.c
    third_party/stb/stb_ds_impl.c
)

# Plugin DLL (pure C)
add_library(endstone_mediaplayer SHARED
    src/plugin.c
    src/music_player.c
    src/sfunc.c
    src/endstone_api.c
    ${THIRD_PARTY_SOURCES}
)

target_include_directories(endstone_mediaplayer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cppcompat/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/nbsparser
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cppcompat/src
)

set_target_properties(endstone_mediaplayer PROPERTIES
    PREFIX "endstone_"
    OUTPUT_NAME "mediaplayer"
)

if(WIN32)
    target_compile_definitions(endstone_mediaplayer PRIVATE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
    )
endif()

# Install to plugins directory (override with -DPLUGINS_DIR=...)
set(PLUGINS_DIR "" CACHE PATH "Endstone plugins directory")

if(PLUGINS_DIR)
    install(TARGETS endstone_mediaplayer
        RUNTIME DESTINATION "${PLUGINS_DIR}"
    )
endif()

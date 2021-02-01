
#pragma once

#if defined(FORGE_DEBUG)
#    define _DEBUG
#    undef NDEBUG
#else
#    define NDEBUG
#    undef _DEBUG
#endif

#if defined(__cplusplus)
#    if __cplusplus <= 199711L
#        define FORGE_CPP_VER 98
#    elif __cplusplus <= 201103L
#        define FORGE_CPP_VER 11
#    elif __cplusplus <= 201402L
#        define FORGE_CPP_VER 14
#    elif __cplusplus <= 201703L
#        define FORGE_CPP_VER 17
#    elif __cplusplus <= 202002L
#        define FORGE_CPP_VER 20
#    else
#        error Unknown C++ version
#    endif
#else
#    error Not C++
#endif

#if !defined(__x86_64__)
#    error Not x86
#endif

#if defined(__clang__)
#    define FORGE_COMPILER_CLANG
#elif defined(_MSC_VER)
#    define FORGE_COMPILER_MSVC
#elif defined(__INTEL_COMPILER)
#    define FORGE_COMPILER_INTEL
#elif defined(__GNUC__)
#    define FORGE_COMPILER_GCC
#else
#    define FORGE_COMPILER_UNKNOWN
#endif

#if defined(_WIN32) || defined(_WIN64)
#    define FORGE_OS_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#    define FORGE_OS_MACOS
#elif defined(__linux__)
#    define FORGE_OS_LINUX
#else
#    define FORGE_OS_UNKNOWN
#endif

#if defined(__unix__) || defined(FORGE_OS_MACOS)
#    include <unistd.h>
#endif

#define FORGE_EVAL_DISCARD(statement) ((void)statement)

#include <cstddef>
#include <cstdint>

namespace forge
{
static constexpr const char* __name__    = "forge";
static constexpr const char* __version__ = "1.0.0";
#if defined(FORGE_DEBUG)
static constexpr bool __debug__ = true;
#else
static constexpr bool __debug__ = false;
#endif

} // namespace forge

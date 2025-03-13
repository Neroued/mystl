//===-------------------------------------===//
//
// config.h
// 定义一些常用的宏
//
//===-------------------------------------===//

#ifndef _MYSTL_CONFIG_H
#define _MYSTL_CONFIG_H

// 命名空间 mystl
#define _MYSTL_BEGIN_NAMESPACE_MYSTL namespace mystl {
#define _MYSTL_END_NAMESPACE_MYSTL   }

// 判断 C++ 版本
#if __cplusplus <= 201103L
#    define _MYSTL_CXX_VERSION 11
#elif __cplusplus <= 201402L
#    define _MYSTL_CXX_VERSION 14
#elif __cplusplus <= 201703L
#    define _MYSTL_CXX_VERSION 17
#elif __cplusplus <= 202002L
#    define _MYSTL_CXX_VERSION 20
#elif __cplusplus <= 202302L
#    define _MYSTL_CXX_VERSION 23
#else
// Expected release year of the next C++ standard
#    define _MYSTL_CXX_VERSION 26
#endif

// 根据 c++ 版本定义合适的宏
#if _MYSTL_CXX_VERSION >= 20
#    define _MYSTL_CONSTEXPR_SINCE_CXX20 constexpr
#else
#    define _MYSTL_CONSTEXPR_SINCE_CXX20
#endif

#if _MYSTL_CXX_VERSION >= 17
#    define _MYSTL_CONSTEXPR_SINCE_CXX17 constexpr
#else
#    define _MYSTL_CONSTEXPR_SINCE_CXX17
#endif

#if _MYSTL_CXX_VERSION >= 14
#    define _MYSTL_CONSTEXPR_SINCE_CXX14 constexpr
#else
#    define _MYSTL_CONSTEXPR_SINCE_CXX14
#endif

// 是否开启异常
#ifdef __cpp_exceptions
#    define _MYSTL_HAS_EXCEPTIONS 1
#else
#    define _MYSTL_HAS_EXCEPTIONS 0
#endif // __cpp_exceptions

#if defined(__clang__) && __clang_major__ >= 8
#    define IS_CONSTANT_EVALUATED() __builtin_is_constant_evaluated()
#else
#    define IS_CONSTANT_EVALUATED() false
#endif

#include <cstddef>
_MYSTL_BEGIN_NAMESPACE_MYSTL

// size_t
using std::size_t;
using std::ptrdiff_t;

_MYSTL_END_NAMESPACE_MYSTL

#endif // _MYSTL_CONFIG_H
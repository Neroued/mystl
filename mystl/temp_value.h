#ifndef _MYSTL_TEMP_VALUE_H
#define _MYSTL_TEMP_VALUE_H

#include <config.h>
#include <memory>
#include <utility>

_MYSTL_BEGIN_NAMESPACE_MYSTL

template <typename _Tp, class _Alloc>
struct __temp_value {
    using alloc_traits = std::allocator_traits<_Alloc>;

    union {
        _Tp __v;
    };

    _Alloc& __alloc_;

    _MYSTL_CONSTEXPR_SINCE_CXX20 _Tp* __addr() { return std::addressof(__v); }

    _MYSTL_CONSTEXPR_SINCE_CXX20 _Tp& get() { return *__addr(); }

    template <class... _Args>
    _MYSTL_CONSTEXPR_SINCE_CXX20 __temp_value(_Alloc& __a, _Args&&... __args) : __alloc_(__a) {
        alloc_traits::construct(__alloc_, __addr(), std::forward<_Args>(__args)...);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 ~__temp_value() { alloc_traits::destroy(__alloc_, __addr()); }
};

_MYSTL_END_NAMESPACE_MYSTL

#endif // _MYSTL_TEMP_VALUE_H
#ifndef _MYSTL_ALLOCATION_GUARD_H
#define _MYSTL_ALLOCATION_GUARD_H

#include <algorithm>
#include <config.h>
#include <memory>

_MYSTL_BEGIN_NAMESPACE_MYSTL

template <class _Alloc>
struct __allocation_guard {
    using pointer   = typename std::allocator_traits<_Alloc>::pointer;
    using size_type = typename std::allocator_traits<_Alloc>::size_type;

    template <class _AllocT>
    explicit __allocation_guard(_AllocT __a, size_type __n)
        : __alloc_(std::move(__a)), __n_(__n), __ptr_(std::allocator_traits<_Alloc>::allocate(__alloc_, __n_)) {}

    ~__allocation_guard() noexcept { __destroy(); }

    __allocation_guard(const __allocation_guard&) = delete;

    __allocation_guard(__allocation_guard&& __other) noexcept : __alloc_(std::move(__other.__alloc_)), __n_(__other.__n_), __ptr_(__other.__ptr_) {
        __other.__ptr_ = nullptr;
    }

    __allocation_guard& operator=(const __allocation_guard&) = delete;

    __allocation_guard& operator=(__allocation_guard&& __other) noexcept {
        if (std::addressof(__other) != this) {
            __destroy();
            __alloc_       = std::move(__other.__alloc_);
            __n_           = __other.__n_;
            __ptr_         = __other.__ptr_;
            __other.__ptr_ = nullptr;
        }
        return *this;
    }

    // 将 __ptr_ 的所有权转移
    [[nodiscard]] pointer __release_ptr() noexcept {
        pointer __tmp = __ptr_;
        __ptr_        = nullptr;
        return __tmp;
    }

    pointer __get() const noexcept { return __ptr_; }

private:
    void __destroy() noexcept {
        if (__ptr_ != nullptr) { std::allocator_traits<_Alloc>::deallocate(__alloc_, __ptr_, __n_); }
    }

    _Alloc __alloc_;
    size_type __n_;
    pointer __ptr_;
};

_MYSTL_END_NAMESPACE_MYSTL

#endif // _MYSTL_ALLOCATION_GUARD_H
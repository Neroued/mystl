//===-------------------------------------===//
//
// allocator.h
// 提供标准化的接口，使用指定Alloc分配内存
//
//===-------------------------------------===//

#ifndef _MYSTL_ALLOCATOR_H
#define _MYSTL_ALLOCATOR_H

#include <allocs.h>
#include <config.h>
#include <iostream>
#include <type_traits>


_MYSTL_BEGIN_NAMESPACE_MYSTL

template <typename _Tp, class _AllocPolicy = alloc>
class allocator {
    static_assert(!std::is_const<_Tp>::value, "mystl::allocator does not support const types");
    static_assert(!std::is_volatile<_Tp>::value, "mystl::allocator does not support volatile types");

public:
    using size_type                              = size_t;
    using difference_type                        = ptrdiff_t;
    using value_type                             = _Tp;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal                        = std::true_type; // deprecated in C++23

    _MYSTL_CONSTEXPR_SINCE_CXX20 allocator() noexcept = default;

    template <typename _Up>
    _MYSTL_CONSTEXPR_SINCE_CXX20 allocator(const allocator<_Up>&) noexcept {}

    [[nodiscard]] _MYSTL_CONSTEXPR_SINCE_CXX20 value_type* allocate(size_type __n) {
        static_assert(sizeof(value_type) >= 0, "cannot allocate memory for an incomplete type");
        if (__n > std::allocator_traits<allocator>::max_size(*this)) throw std::bad_array_new_length();
        std::cout << "[mystl::allocator]: allocate " << __n << std::endl;
        return static_cast<value_type*>(alloc::allocate(__n * sizeof(value_type)));
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void deallocate(value_type* __p, size_type __n) noexcept {
        alloc::deallocate(__p, __n);
        std::cout << "[mystl::allocator]: deallocate " << __n << std::endl;
    }

    // 以下在 C++20 中移除
#if _MYSTL_CXX_VERSION <= 17
    using pointer         = _Tp*;
    using const_pointer   = const _Tp*;
    using reference       = _Tp&;
    using const_reference = const _Tp&;

    template <typename _Up>
    struct rebind {
        using other = allocator<_Up>;
    };

    pointer address(reference __x) const noexcept {
        return std::addressof(__x);
    }

    const_pointer address(const_reference __x) const noexcept {
        return std::addressof(__x);
    }

    [[nodiscard]] _Tp* allocate(size_t __n, const void*) {
        return allocate(__n);
    }

    size_type max_size() const noexcept {
        return size_type(~0) / sizeof(_Tp);
    }

    // 使用placement new, 在分配好的内存上构造对象
    // 语法为 new (address) Type (constructor_arguments)
    template <typename _Up, class... _Args>
    void construct(_Up* __p, _Args&&... __args) {
        ::new ((void*)__p) _Up(std::forward<_Args>(__args)...);
    }

    void destroy(pointer __p) {
        __p->~_Tp();
    }

#endif // _MYSTL_CXX_VERSION <= 17
};

template <typename _Tp, typename _Up>
inline _MYSTL_CONSTEXPR_SINCE_CXX20 bool operator==(const allocator<_Tp>&, const allocator<_Up>&) noexcept {
    return true;
}

#if _MYSTL_CXX_VERSION <= 17

template <typename _Tp, typename _Up>
inline bool operator!=(const allocator<_Tp>&, const allocator<_Up>&) noexcept {
    return false;
}

#endif // _MYSTL_CXX_VERSION <= 17

// 用于判断是不是 allocator 的模板
template <typename T, typename = void>
struct is_allocator : std::false_type {};

template <typename T>
struct is_allocator<T,
                    std::void_t<decltype(std::declval<T&>().allocate(std::size_t(1))),
                                decltype(std::declval<T&>().deallocate(std::declval<typename std::allocator_traits<T>::pointer>(), std::size_t(1)))>>
    : std::true_type {};

// 方便使用的 helper
template <typename T>
constexpr bool is_allocator_v = is_allocator<T>::value;

_MYSTL_END_NAMESPACE_MYSTL

#endif // _MYSTL_ALLOCATOR_H

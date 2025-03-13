#ifndef _MYSTL_UNITIALIZED_ALGORITHMS_H
#define _MYSTL_UNITIALIZED_ALGORITHMS_H

#include <config.h>
#include <cstring>
#include <exception_guard.h>
#include <iterator.h>
#include <type_traits>


_MYSTL_BEGIN_NAMESPACE_MYSTL

template <class _Alloc, class _Iter>
class _AllocatorDestroyRangeReverse {
public:
    _MYSTL_CONSTEXPR_SINCE_CXX14 _AllocatorDestroyRangeReverse(_Alloc& __alloc, _Iter& __first, _Iter& __last)
        : __alloc_(__alloc), __first_(__first), __last_(__last) {}

    _MYSTL_CONSTEXPR_SINCE_CXX14 void operator()() const {
        auto rfirst = reverse_iterator<_Iter>(__last_);
        auto rlast  = reverse_iterator<_Iter>(__first_);
        for (; rfirst != rlast; ++rfirst) { std::allocator_traits<_Alloc>::destroy(__alloc_, std::addressof(*rfirst)); }
    }

private:
    _Alloc& __alloc_;
    _Iter& __first_; // 存储引用，因为创建后外部迭代器仍然会变化，需要保持一致
    _Iter& __last_;
};

// 将 [__first, __last) 中的元素迁移到 __result 开始的位置
template <class _Alloc, class _ContiguousIterator>
_MYSTL_CONSTEXPR_SINCE_CXX14 void __uninitialized_allocator_relocate(_Alloc& __alloc_, _ContiguousIterator __first, _ContiguousIterator __last,
                                                                     _ContiguousIterator __result) {
    using _ValueType = typename iterator_traits<_ContiguousIterator>::value_type;

    // 根据类型选择不同的迁移方式
    if (IS_CONSTANT_EVALUATED() || !std::is_trivially_copyable<_ValueType>::value || !std::is_nothrow_move_constructible<_ValueType>::value ||
        !std::is_trivially_destructible<_ValueType>::value) {
        auto __destruct_first = __result;
        auto __guard = __make_exception_guard(_AllocatorDestroyRangeReverse<_Alloc, _ContiguousIterator>(__alloc_, __destruct_first, __result));

        // 迁移数据
        auto __iter = __first;
        while (__iter != __last) {
#if _MYSTL_HAS_EXCEPTIONS
            std::allocator_traits<_Alloc>::construct(__alloc_, std::addressof(*__result), std::move_if_noexcept(*__iter));
#else
            std::allocator_traits<_Alloc>::construct(__alloc_, std::addressof(*__result), std::move(*__iter));
#endif //_MYSTL_HAS_EXCEPTIONS
            ++__iter;
            ++__result;
        }
        __guard.__complete();
        // 清除原数据
        for (; __first != __last; ++__first) { std::allocator_traits<_Alloc>::destroy(__alloc_, std::addressof(*__first)); }
    } else {
        // 直接使用 memcpy
        std::memcpy(static_cast<void*>(std::addressof(*__result)), std::addressof(*__first), sizeof(_ValueType) * (__last - __first));
    }
}

// 将 [__first1, __last1) 中的元素拷贝构造到 __first2 开始的 N 个位置，其中 N 是 __first1 到 __last1 之间的距离
// 使用时需要确保 __first2 开始有足够大小的空间
template <class _Alloc, class _Iter1, class _Sent1, class _Iter2>
_MYSTL_CONSTEXPR_SINCE_CXX20 _Iter2 __unintialized_allocator_copy(_Alloc& __alloc, _Iter1 __first1, _Sent1 __last1, _Iter2 __first2) {
    return __unintialized_allocator_copy_impl(__alloc, __first1, __last1, __first2);
}

// 对于一般的类型，逐个拷贝构造
template <class _Alloc, class _Iter1, class _Sent1, class _Iter2>
_MYSTL_CONSTEXPR_SINCE_CXX20 _Iter2 __unintialized_allocator_copy_impl(_Alloc& __alloc, _Iter1 __first1, _Sent1 __last1, _Iter2 __first2) {
    auto __destruct_first = __first2;
    auto __guard          = __make_exception_guard(_AllocatorDestroyRangeReverse<_Alloc, _Iter2>(__alloc, __destruct_first, __first2));
    while (__first1 != __last1) {
        std::allocator_traits<_Alloc>::construct(__alloc, std::addressof(*__first2), *__first1);
        ++__first1;
        ++__first2;
    }

    __guard.__complete();
    return __first2;
}

// TODO: 实现 unwrap_iter
// 可以使用 unwrap_iter 将指针从 iter 中解包，若是指针类型可以有更强的优化

// C++20 中对 contiguous_iterator 以及 trivial 类型使用 memcpy 优化
#if _MYSTL_CXX_VERSION >= 20
template <class _Alloc, ExactContiguousIterator _In, ExactContiguousIterator _Out,
          std::enable_if_t<std::is_same_v<typename std::iterator_traits<_In>::value_type, typename std::iterator_traits<_Out>::value_type> &&
                               std::is_trivially_copyable_v<typename std::iterator_traits<_In>::value_type> &&
                               std::is_nothrow_move_constructible_v<typename std::iterator_traits<_In>::value_type> &&
                               std::is_trivially_destructible_v<typename std::iterator_traits<_In>::value_type>,
                           int> = 0>
_MYSTL_CONSTEXPR_SINCE_CXX20 _Out __unintialized_allocator_copy_impl(_Alloc& __alloc, _In __first1, _In __last1, _Out __first2) {
    if (IS_CONSTANT_EVALUATED()) {
        while (__first1 != __last1) {
            std::allocator_traits<_Alloc>::construct(__alloc, std::addressof(*__first2), *__first1);
            ++__first1;
            ++__first2;
        }
        return __first2;
    } else {
        size_t __n = static_cast<size_t>(std::distance(__first1, __last1));
        std::memcpy(static_cast<void*>(std::addressof(*__first2)), std::addressof(*__first1),
                    __n * sizeof(typename std::iterator_traits<_In>::value_type));
        return __first2 + __n;
    }
}
#endif //_MYSTL_CXX_VERSION >= 20

_MYSTL_END_NAMESPACE_MYSTL

#endif
#ifndef _MYSTL_VECTOR_H
#define _MYSTL_VECTOR_H

#include <algorithm>
#include <allocator.h>
#include <config.h>
#include <exception_guard.h>
#include <initializer_list>
#include <iterator.h>
#include <temp_value.h>
#include <uninitialized_algorithms.h>


_MYSTL_BEGIN_NAMESPACE_MYSTL

template <typename _Tp, class _Allocator = allocator<_Tp>>
class vector {
public:
    using value_type             = _Tp;
    using allocator_type         = _Allocator;
    using alloc_traits           = std::allocator_traits<allocator_type>;
    using reference              = value_type&;                      // 根据标准，引用类型一定是 value_type&
    using const_reference        = const value_type&;
    using size_type              = typename alloc_traits::size_type; // 但 allocator 可以自定义这四个类型
    using difference_type        = typename alloc_traits::difference_type;
    using pointer                = typename alloc_traits::pointer;   // 例如这可能是 std::unique_ptr<_Tp>
    using const_pointer          = typename alloc_traits::const_pointer;
    using iterator               = wrap_iter<pointer>;
    using const_iterator         = wrap_iter<const_pointer>;
    using reverse_iterator       = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    static_assert(std::is_same_v<typename allocator_type::value_type, value_type>, "Allocator::value_type must be same type as value_type");

private:
    pointer __begin_ = nullptr;
    pointer __end_   = nullptr;
    pointer __cap_   = nullptr;
    allocator_type __alloc_;

public:
    //
    // [vector.cons] constuct/copy/destroy
    //
    // 构造自定义的 __alloc_ 可能会抛出异常
    _MYSTL_CONSTEXPR_SINCE_CXX20 vector() noexcept(std::is_nothrow_default_constructible<allocator_type>::value) {}

    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(const allocator_type& __a) noexcept : __alloc_(__a) {} // 在 C++17 及以后，拷贝构造分配器不会造成异常

    _MYSTL_CONSTEXPR_SINCE_CXX20 explicit vector(size_type __n) {
        auto __guard = __make_exception_guard(__destroy_vector(*this));
        if (__n > 0) {
            __vallocate(__n);
            __construct_at_end(__n);
        }
        __guard.__complete();
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 explicit vector(size_type __n, const allocator_type& __a) : __alloc_(__a) {
        auto __guard = __make_exception_guard(__destroy_vector(*this));
        if (__n > 0) {
            __vallocate(__n);
            __construct_at_end(__n);
        }
        __guard.__complete();
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(size_type __n, const_reference __x) {
        auto __guard = __make_exception_guard(__destroy_vector(*this));
        if (__n > 0) {
            __vallocate(__n);
            __construct_at_end(__n, __x);
        }
        __guard.__complete();
    }

    template <std::enable_if_t<is_allocator<_Allocator>::value, int> = 0>
    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(size_type __n, const_reference __x, const allocator_type& __a) : __alloc_(__a) {
        auto __guard = __make_exception_guard(__destroy_vector(*this));
        if (__n > 0) {
            __vallocate(__n);
            __construct_at_end(__n, __x);
        }
        __guard.__complete();
    }

// 使用迭代器构造
// input_iterator 必须一个个进行构造，且无法预先知道长度
#if _MYSTL_CXX_VERSION <= 17
    template <class _InputIterator,
              std::enable_if_t<is_exact_input_iterator<_InputIterator>::value &&
                                   std::is_constructible<value_type, typename iterator_traits<_InputIterator>::reference>::value,
                               int> = 0>
#else
    template <ExactInputIterator _InputIterator,
              std::enable_if_t<std::is_constructible_v<value_type, typename iterator_traits<_InputIterator>::reference>, int> = 0>
#endif
    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(_InputIterator __first, _InputIterator __last) {
        __init_with_sentinel(__first, __last);
    }

#if _MYSTL_CXX_VERSION <= 17
    template <class _InputIterator,
              std::enable_if_t<is_exact_input_iterator<_InputIterator>::value &&
                                   std::is_constructible<value_type, typename iterator_traits<_InputIterator>::reference>::value,
                               int> = 0>
#else
    template <ExactInputIterator _InputIterator,
              std::enable_if_t<std::is_constructible_v<value_type, typename iterator_traits<_InputIterator>::reference>, int> = 0>
#endif
    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(_InputIterator __first, _InputIterator __last, const allocator_type& __a) : __alloc_(__a) {
        __init_with_sentinel(__first, __last);
    }

// 强于 input_iterator 的可以提前知道长度，且可以多次访问
// 在 __init_with_size 内部使用的算法 __unintialized_allocator_copy 在 C++20 下对 contiguous_iterator 有优化
#if _MYSTL_CXX_VERSION <= 17
    template <class _ForwardIterator, std::enable_if_t<is_based_on_forward_iterator<_ForwardIterator>::value &&
                                                           std::is_constructible_v<value_type, typename iterator_traits<_ForwardIterator>::reference>,
                                                       int> = 0>
#else
    template <BasedOnForwardIterator _ForwardIterator,
              std::enable_if_t<std::is_constructible_v<value_type, typename iterator_traits<_ForwardIterator>::reference>, int> = 0>
#endif
    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(_ForwardIterator __first, _ForwardIterator __last) {
        size_type __n = static_cast<size_type>(std::distance(__first, __last));
        __init_with_size(__first, __last, __n);
    }

#if _MYSTL_CXX_VERSION <= 17
    template <class _ForwardIterator, std::enable_if_t<is_based_on_forward_iterator<_ForwardIterator>::value &&
                                                           std::is_constructible_v<value_type, typename iterator_traits<_ForwardIterator>::reference>,
                                                       int> = 0>
#else
    template <BasedOnForwardIterator _ForwardIterator,
              std::enable_if_t<std::is_constructible_v<value_type, typename iterator_traits<_ForwardIterator>::reference>, int> = 0>
#endif
    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(_ForwardIterator __first, _ForwardIterator __last, const allocator_type& __a) : __alloc_(__a) {
        size_type __n = static_cast<size_type>(std::distance(__first, __last));
        __init_with_size(__first, __last, __n);
    }

    // 拷贝构造
    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(const vector& __other) : __alloc_(alloc_traits::select_on_container_copy_construction(__other.__alloc_)) {
        __init_with_size(__other.__begin_, __other.__end_, __other.size());
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(const vector& __other, const allocator_type& __a) : __alloc_(__a) {
        __init_with_size(__other.__begin_, __other.__end_, __other.size());
    }

    // 使用 initializer_list 构造
    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(std::initializer_list<value_type> __il) {
        __init_with_size(__il.begin(), __il.end(), __il.size());
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(std::initializer_list<value_type> __il, const allocator_type& __a) : __alloc_(__a) {
        __init_with_size(__il.begin(), __il.end(), __il.size());
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 vector& operator=(std::initializer_list<value_type> __il) {
        assign(__il.begin(), __il.end());
        return *this;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 vector(vector&& __x) noexcept : __alloc_(std::move(__x.__alloc_)) {
        __begin_     = __x.__begin_;
        __end_       = __x.__end_;
        __cap_       = __x.__cap_;
        __x.__begin_ = __x.__end_ = __x.__cap_ = nullptr;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 vector& operator=(vector&& __x) noexcept(alloc_traits::propagate_on_container_move_assignment::value ||
                                                                          alloc_traits::is_always_equal::value) {
        __move_assign(__x, std::integral_constant<bool, alloc_traits::propagate_on_container_move_assignment::value>());
        return *this;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 vector& operator=(const vector& __x) {
        if (this != std::addressof(__x)) {
            __copy_assign_alloc(__x);
            assign(__x.begin(), __x.end());
        }
        return *this;
    }

#if _MYSTL_CXX_VERSION <= 17
    template <class _InputIterator,
              std::enable_if_t<is_exact_input_iterator<_InputIterator>::value &&
                                   std::is_constructible<value_type, typename iterator_traits<_InputIterator>::reference>::value,
                               int> = 0>
#else
    template <ExactInputIterator _InputIterator,
              std::enable_if_t<std::is_constructible_v<value_type, typename iterator_traits<_InputIterator>::reference>, int> = 0>
#endif
    _MYSTL_CONSTEXPR_SINCE_CXX20 void assign(_InputIterator __first, _InputIterator __last) {
        __assign_with_sentinel(__first, __last);
    }

#if _MYSTL_CXX_VERSION <= 17
    template <class _ForwardIterator, std::enable_if_t<is_based_on_forward_iterator<_ForwardIterator>::value &&
                                                           std::is_constructible_v<value_type, typename iterator_traits<_ForwardIterator>::reference>,
                                                       int> = 0>
#else
    template <BasedOnForwardIterator _ForwardIterator,
              std::enable_if_t<std::is_constructible_v<value_type, typename iterator_traits<_ForwardIterator>::reference>, int> = 0>
#endif
    _MYSTL_CONSTEXPR_SINCE_CXX20 void assign(_ForwardIterator __first, _ForwardIterator __last) {
        __assign_with_size(__first, __last, std::distance(__first, __last));
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void assign(size_type __n, const_reference __x) {
        if (__n <= capacity()) {
            size_type __old_size = size();
            std::fill_n(__begin_, std::min(__n, __old_size), __x);
            if (__n <= __old_size) {
                __base_destruct_at_end(__begin_ + __n);
            } else {
                __construct_at_end(__n - __old_size, __x);
            }
        } else {
            __vdeallocate();
            __vallocate(__recommend(__n));
            __construct_at_end(__n, __x);
        }
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void assign(std::initializer_list<value_type> __il) {
        assign(__il.begin(), __il.end());
    }

private:
    // 辅助析构的类
    class __destroy_vector {
    public:
        constexpr __destroy_vector(vector& __vec) : __vec_(__vec) {}

        // 重载 operator(), 执行 vector 的析构
        _MYSTL_CONSTEXPR_SINCE_CXX20 void operator()() {
            if (__vec_.__begin_ != nullptr) { alloc_traits::deallocate(__vec_.__alloc_, std::addressof(*__vec_.__begin_), __vec_.capacity()); }
        }

    private:
        vector& __vec_;
    };

public:
    _MYSTL_CONSTEXPR_SINCE_CXX20 ~vector() {
        clear();
        __destroy_vector (*this)();
    }

    // assign


    _MYSTL_CONSTEXPR_SINCE_CXX20 allocator_type get_allocator() const noexcept {
        return __alloc_;
    }

    //
    // Iterators
    //
    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator begin() noexcept {
        return __make_iter(__begin_);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_iterator begin() const noexcept {
        return __make_iter(__begin_);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator end() noexcept {
        return __make_iter(__end_);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_iterator end() const noexcept {
        return __make_iter(__end_);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_iterator cbegin() const noexcept {
        return begin();
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_iterator cend() const noexcept {
        return end();
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_reverse_iterator crend() const noexcept {
        return rend();
    }

    //
    // [vector.capacity], capacity
    //
    _MYSTL_CONSTEXPR_SINCE_CXX20 size_type size() const noexcept {
        return static_cast<size_type>(__end_ - __begin_);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 size_type capacity() const noexcept {
        return static_cast<size_type>(__cap_ - __begin_);
    }

    [[nodiscard]] _MYSTL_CONSTEXPR_SINCE_CXX20 bool empty() const noexcept {
        return __begin_ == __end_;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 size_type max_size() const noexcept {
        return std::min<size_type>(alloc_traits::max_size(this->__alloc_), std::numeric_limits<difference_type>::max());
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void reserve(size_type __n) {
        if (__n > capacity()) {
            if (__n > max_size()) { throw std::length_error("vector"); }
            __reallocation_buffer __buffer(__alloc_, __n);
            __swap_reallocation_buffer(__buffer);
        }
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void shrink_to_fit() noexcept {
        if (capacity() > size()) {
#if _MYSTL_HAS_EXCEPTIONS
            try {
#endif
                __reallocation_buffer __buffer(__alloc_, size());
                __swap_reallocation_buffer(__buffer);
#if _MYSTL_HAS_EXCEPTIONS
            } catch (...) {}
#endif
        }
    }

    // element access
    _MYSTL_CONSTEXPR_SINCE_CXX20 reference operator[](size_type __n) noexcept {
        return __begin_[__n];
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_reference operator[](size_type __n) const noexcept {
        return __begin_[__n];
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 reference at(size_type __n) {
        if (__n >= size()) { throw std::out_of_range("vector"); }
        return __begin_[__n];
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_reference at(size_type __n) const {
        if (__n >= size()) { throw std::out_of_range("vector"); }
        return __begin_[__n];
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 reference front() noexcept {
        return *__begin_;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_reference front() const noexcept {
        return *__begin_;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 reference back() noexcept {
        return *(__end_ - 1);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_reference back() const noexcept {
        return *(__end_ - 1);
    }

    //
    // [vector.data], data access
    _MYSTL_CONSTEXPR_SINCE_CXX20 value_type* data() noexcept {
        return std::addressof(*__begin_);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const value_type* data() const noexcept {
        return std::addressof(*__begin_);
    }

    //
    // [vector.modifiers], modifiers
    //
    _MYSTL_CONSTEXPR_SINCE_CXX20 void push_back(const_reference __x) {
        emplace_back(__x);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void push_back(value_type&& __x) {
        emplace_back(std::move(__x));
    }

    // emplace_back 分为触发扩容与不触发扩容两个函数
    template <class... _Args>
    reference emplace_back(_Args... __args) {
        if (__end_ < __cap_) {
            __construct_one_at_end(std::forward<_Args>(__args)...);
        } else {
            __end_ = __emplace_back_slow_path(std::forward<_Args>(__args)...);
        }
        return *(__end_ - 1);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void pop_back() {
        __base_destruct_at_end(__end_ - 1);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator __position, const_reference __x) {
        pointer __p = __begin_ + (__position - begin());
        if (__end_ < __cap_) { // 容量足够
            if (__p == __end_) {
                __construct_one_at_end(__x);
            } else {
                // 将 [__p, __end_) 的元素向后移一位
                __move_range(__p, __end_, __p + 1);
                // __x 有可能本身是 vector 中的元素，因此 __move_range 可能修改了 __x，因此需要进行修正
                // 通过判断指向 __x 的指针是否在 [__p, __end_)
                const_pointer __xr = std::pointer_traits<const_pointer>::pointer_to(__x);
                if (__p <= __xr && __end_ >= __xr) // 直接比较地址
                {
                    ++__xr;
                }
                *__p = *__xr;
            }
        } else { // 扩容
            __reallocation_buffer __buffer(__alloc_, __recommend(size() + 1));
            // 先在 __p 对应位置 __new_p 构造元素
            pointer __new_p = __buffer.__begin_ + (__position - begin());
            alloc_traits::construct(__buffer.__alloc_, __new_p, std::move(__x));
            // 分别移动 [__begin_, __p) 和 [__p + 1, __end_) 的元素
            __swap_reallocation_buffer(__buffer, __p, 1);
            __p = __new_p;
        }

        return __make_iter(__p);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator __position, value_type&& __x) {
        pointer __p = __begin_ + (__position - begin());
        if (__end_ < __cap_) { // 容量足够
            if (__p == __end_) {
                __construct_one_at_end(std::move(__x));
            } else {
                // 将 [__p, __end_) 的元素向后移一位
                __move_range(__p, __end_, __p + 1);
                *__p = std::move(__x);
            }
        } else { // 扩容
            __reallocation_buffer __buffer(__alloc_, __recommend(size() + 1));
            // 先在 __p 对应位置 __new_p 构造元素
            pointer __new_p = __buffer.__begin_ + (__position - begin());
            alloc_traits::construct(__buffer.__alloc_, __new_p, std::move(__x));
            // 分别移动 [__begin_, __p) 和 [__p + 1, __end_) 的元素
            __swap_reallocation_buffer(__buffer, __p, 1);
            __p = __new_p;
        }

        return __make_iter(__p);
    }

    //
    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator __position, size_type __n, const_reference __x) {
        pointer __p = __begin_ + (__position - begin());
        if (__n > 0) {
            if (!IS_CONSTANT_EVALUATED() && __n <= static_cast<size_type>(__cap_ - __end_)) {
                size_type __old_n  = __n;
                pointer __old_last = __end_;
                if (__n > static_cast<size_type>(__end_ - __p)) { // 需要插入的 __n 个元素已经超出了 __end_ 需要重新构造
                    size_type __count_x = __n - static_cast<size_type>(__end_ - __p);
                    __construct_at_end(__count_x, __x);
                    __n -= __count_x;
                }
                if (__n > 0) {
                    __move_range(__p, __old_last, __p + __old_n); // 将原本元素移动
                    // 处理 __x 在被移动的元素中的情况
                    const_pointer __xr = std::pointer_traits<const_pointer>::pointer_to(__x);
                    if (__p <= __xr && __end_ >= __xr) { __xr += __old_n; }
                    std::fill_n(__p, __n, *__xr);
                }
            } else { // 扩容
                __reallocation_buffer __buffer(__alloc_, __recommend(size() + __n));
                pointer __new_p = __buffer.__begin_ + (__position - begin());
                __buffer.__construct_at(__new_p, __n, __x);
                __swap_reallocation_buffer(__buffer, __p, __n);
                __p = __new_p;
            }
        }
        return __make_iter(__p);
    }

#if _MYSTL_CXX_VERSION <= 17
    template <class _InputIterator,
              std::enable_if_t<is_exact_input_iterator<_InputIterator>::value &&
                                   std::is_constructible<value_type, typename iterator_traits<_InputIterator>::reference>::value,
                               int> = 0>
#else
    template <ExactInputIterator _InputIterator,
              std::enable_if_t<std::is_constructible_v<value_type, typename iterator_traits<_InputIterator>::reference>, int> = 0>
#endif
    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator __position, _InputIterator __first, _InputIterator __last) {
        // 输入迭代器无法知道具体的元素数量，需要先尝试进行构造
        difference_type __offset = __position - begin();
        pointer __p              = __begin_ + __offset;
        pointer __old_last       = __end_;
        // 尝试在剩余空间中先构造元素
        for (; __end_ != __cap_ && __first != __last; ++__first) { __construct_one_at_end(*__first); }

        if (__first == __last) { // 若已经插入完成则将新构造的元素移动到正确位置
            std::rotate(__p, __old_last, __end_);
        } else {                 // 扩容
            // 开辟一个缓冲区用于构造剩余要插入的元素
            __reallocation_buffer __buffer(__alloc_);
            auto __guard = __make_exception_guard(_AllocatorDestroyRangeReverse<allocator_type, pointer>(__alloc_, __old_last, __end_));
            __buffer.__construct_at_end_with_sentinel(std::move(__first), std::move(__last));

            size_type __count = static_cast<size_type>(__cap_ - __old_last); // 在剩余空间中构造的元素数量
            size_type __n     = __count + __buffer.size();                   // 插入的元素数量

            // 开辟缓冲区存放最终的全部元素
            __reallocation_buffer __merged(__alloc_, __recommend(size() + __buffer.size()));

            // 移动在剩余空间中构造的元素
            __uninitialized_allocator_relocate(__alloc_, __old_last, __end_, __merged.__begin_ + __offset);
            __end_ = __old_last;
            __guard.__complete();

            // 移动在 __buffer 中构造的元素
            __uninitialized_allocator_relocate(__alloc_, __buffer.__begin_, __buffer.__end_, __merged.__begin_ + __offset + __count);
            __buffer.__end_ = __buffer.__begin_;
            // 移动原有的元素

            __swap_reallocation_buffer(__merged, __p, __n);
            __p = __begin_ + __offset;
        }
        return __make_iter(__p);
    }

#if _MYSTL_CXX_VERSION <= 17
    template <class _ForwardIterator, std::enable_if_t<is_based_on_forward_iterator<_ForwardIterator>::value &&
                                                           std::is_constructible_v<value_type, typename iterator_traits<_ForwardIterator>::reference>,
                                                       int> = 0>
#else
    template <BasedOnForwardIterator _ForwardIterator,
              std::enable_if_t<std::is_constructible_v<value_type, typename iterator_traits<_ForwardIterator>::reference>, int> = 0>
#endif
    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator __position, _ForwardIterator __first, _ForwardIterator __last) {
        // 前向迭代器可以计算输入元素的数量
        difference_type __offset = __position - begin();
        pointer __p              = __begin_ + __offset;
        size_type __n            = std::distance(__first, __last);
        if (__n > 0) {
            if (__n <= __cap_ - __end_) {
                pointer __old_last = __end_;
                size_type __old_n  = __n;
                if (__n > static_cast<size_type>(__end_ - __p)) {              // 插入的元素超出了 __end_, 需要重新构造
                    size_type __count_x = static_cast<size_type>(__end_ - __p);
                    auto __m            = std::next(__first, __n - __count_x); // __first 的第 __n - __count_x 个后继
                    __construct_at_end(__m, __last, __count_x);
                    if (__count_x > 0) {
                        __move_range(__p, __old_last, __p + __n);
                        std::copy(__first, __m, __p);
                    }
                } else {
                    __move_range(__p, __old_last, __p + __n);
                    std::copy_n(__first, __n, __p);
                }
            } else {
                __reallocation_buffer __buffer(__alloc_, __recommend(size() + __n));
                pointer __new_p = __buffer.__begin_ + __offset;
                __buffer.__construct_at(__new_p, __first, __n);
                __swap_reallocation_buffer(__buffer, __p, __n);
                __p = __new_p;
            }
        }
        return __make_iter(__p);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator insert(const_iterator __position, std::initializer_list<value_type> __il) {
        return insert(__position, __il.begin(), __il.end());
    }

    template <class... _Args>
    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator emplace(const_iterator __position, _Args&&... __args) {
        difference_type __offset = __position - begin();
        pointer __p              = __begin_ + __offset;
        if (__end_ < __cap_) {
            if (__p == __end_) {
                __construct_one_at_end(std::forward<_Args>(__args)...);
            } else {
                __temp_value<value_type, _Allocator> __tmp(__alloc_, std::forward<_Args>(__args)...);
                __move_range(__p, __end_, __p + 1);
                *__p = std::move(__tmp.get());
            }
        } else {
            __reallocation_buffer __buffer(__alloc_, __recommend(size() + 1));
            pointer __new_p = __buffer.__begin_ + __offset;
            __buffer.__construct_at(__new_p, std::forward<_Args>(__args)...);
            __swap_reallocation_buffer(__buffer, __p, 1);
            __p = __new_p;
        }
        return __make_iter(__p);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator erase(const_iterator __position) {
        pointer __p = __begin_ + (__position - begin());
        __base_destruct_at_end(std::move(__p + 1, __end_, __p));
        return __make_iter(__p);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator erase(const_iterator __first, const_iterator __last) {
        pointer __p = __begin_ + (__first - begin());
        if (__first != __last) { __base_destruct_at_end(std::move(__p + (__last - __first), __end_, __p)); }
        return __make_iter(__p);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void clear() noexcept {
        __base_destruct_at_end(__begin_);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void resize(size_type __size) {
        size_type __current_size = size();
        if (__current_size < __size) {
            __reallocation_buffer __buffer(__alloc_, __size);
            __buffer.__construct_at(__buffer.__begin_ + __current_size, __size - __current_size);
            __swap_reallocation_buffer(__buffer);
            __end_ = __begin_ + __size;
        } else if (__current_size > __size) {
            __base_destruct_at_end(__begin_ + __size);
        }
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void resize(size_type __size, const_reference __x) {
        size_type __current_size = size();
        if (__current_size < __size) {
            __reallocation_buffer __buffer(__alloc_, __size);
            __buffer.__construct_at(__buffer.__begin_ + __current_size, __size - __current_size, __x);
            __swap_reallocation_buffer(__buffer);
            __end_ = __begin_ + __size;
        } else if (__current_size > __size) {
            __base_destruct_at_end(__begin_ + __size);
        }
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void swap(vector& __other) noexcept {
        static_assert(alloc_traits::propagate_on_container_swap::value || __alloc_ == __other.__alloc_,
                      "vector::swap: Either propagate_on_container_swap must be true"
                      " or the allocators must compare equal");
        std::swap(__begin_, __other.__begin_);
        std::swap(__end_, __other.__end_);
        std::swap(__cap_, __other.__cap_);
        std::swap(__alloc_, __other.__alloc_);
    }

private:
    // Allocate space for __n objects
    // throw length error if __n > max_size()
    // Precondition: __begin_ == __end_ == __cap_ == nullptr
    // Precondition: __n > 0
    // Postcondition: capacity() >= __n
    // Postcondition: size() == 0
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __vallocate(size_type __n) {
        if (__n > max_size()) { throw std::length_error("vector"); }
        __begin_ = __alloc_.allocate(__n);
        __end_   = __begin_;
        __cap_   = __begin_ + __n;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void __vdeallocate() noexcept {
        if (__begin_ != nullptr) {
            clear();
            __alloc_.deallocate(std::addressof(*__begin_), capacity());
            __begin_ = __end_ = __cap_ = nullptr;
        }
    }

    // 容量变化逻辑
    // 总体上将容量翻倍
    // Precondition: __new_size > capacity()
    _MYSTL_CONSTEXPR_SINCE_CXX20 inline size_type __recommend(size_type __new_size) const {
        const size_type __ms = max_size();
        if (__new_size > __ms) { throw std::length_error("vector"); }
        const size_type __cap = capacity();
        if (__cap >= __ms / 2) { return __ms; }
        return std::max<size_type>(2 * __cap, __new_size);
    }

    // 一个辅助用的结构体，计算 construct 时各个指针的变化情况
    // 注意 __pos_ 的位置需要在外部使用时步进
    struct _ConstructTransaction {
        // 构造时计算开始的位置 __pos_ 和结束位置 __new_end_
        _MYSTL_CONSTEXPR_SINCE_CXX20 explicit _ConstructTransaction(vector& __v, size_type __n)
            : __v_(__v), __pos_(__v.__end_), __new_end_(__v.__end_ + __n) {}

        // 析构时修改对应 vector 的 __end_
        _MYSTL_CONSTEXPR_SINCE_CXX20 ~_ConstructTransaction() {
            __v_.__end_ = __pos_;
        }

        vector& __v_;
        pointer __pos_;
        const_pointer const __new_end_;

        _ConstructTransaction(_ConstructTransaction const&)            = delete;
        _ConstructTransaction& operator=(_ConstructTransaction const&) = delete;
    };

    // Default constructs __n objects starting at __end_
    // throws if construction throws
    // Precondition: __n > 0
    // Precondition: size() + __n <= capacity()
    // Postcondition: size() == old size() + __n
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __construct_at_end(size_type __n) {
        _ConstructTransaction __tx(*this, __n);
        for (; __tx.__pos_ != __tx.__new_end_; ++__tx.__pos_) { alloc_traits::construct(__alloc_, std::addressof(*__tx.__pos_)); }
    }

    // Copy constructs __n objects at __end_ from __x
    // throws if construction throws
    // Precondition: __n > 0
    // Precondition: size() + __n <= capacity()
    // Postcondition: size() == old size() + __n
    // Postcondition: [i] == __x for all i in [size()- __n, size())
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __construct_at_end(size_type __n, const_reference __x) {
        _ConstructTransaction __tx(*this, __n);
        for (; __tx.__pos_ != __tx.__new_end_; ++__tx.__pos_) { alloc_traits::construct(__alloc_, std::addressof(*__tx.__pos_), __x); }
    }

    template <class _InputIterator, class _Sentinel>
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __construct_at_end(_InputIterator __first, _Sentinel __last, size_type __n) {
        _ConstructTransaction __tx(*this, __n);
        __tx.__pos_ = __unintialized_allocator_copy(__alloc_, std::move(__first), std::move(__last), __tx.__pos_);
    }

    // Construct one object at __end_
    // throws if construction throws
    // Precondition: size() + 1 <= capacity()
    template <class... _Args>
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __construct_one_at_end(_Args&&... __args) {
        _ConstructTransaction __tx(*this, 1);
        alloc_traits::construct(__alloc_, std::addressof(*__tx.__pos_), std::forward<_Args>(__args)...);
        ++__tx.__pos_;
    }

    // 触发扩容时使用的临时存储区，在这里获取扩容后的空间，再将原数据迁移过来，最后交换指针
    struct __reallocation_buffer {
        pointer __begin_;
        pointer __end_;
        pointer __cap_;
        allocator_type& __alloc_;

        // 删除拷贝构造，拷贝赋值
        __reallocation_buffer(const __reallocation_buffer&)            = delete;
        __reallocation_buffer& operator=(const __reallocation_buffer&) = delete;

        // 析构时需要处理 vector 中的数据
        _MYSTL_CONSTEXPR_SINCE_CXX20 void clear() noexcept {
            if constexpr (!std::is_trivially_destructible_v<value_type>) {
                pointer __soon_to_be_end = __end_;
                while (__begin_ != __soon_to_be_end) { alloc_traits::destroy(__alloc_, std::addressof(*--__soon_to_be_end)); }
            }
            __end_ = __begin_;
        }

        _MYSTL_CONSTEXPR_SINCE_CXX20 __reallocation_buffer(allocator_type& __alloc)
            : __begin_(nullptr), __end_(nullptr), __cap_(nullptr), __alloc_(__alloc) {}

        _MYSTL_CONSTEXPR_SINCE_CXX20 __reallocation_buffer(allocator_type& __alloc, size_type __cap)
            : __begin_(nullptr), __end_(nullptr), __cap_(nullptr), __alloc_(__alloc) {
            __begin_ = __alloc_.allocate(__cap);
            __end_   = __begin_;
            __cap_   = __begin_ + __cap;
        }

        _MYSTL_CONSTEXPR_SINCE_CXX20 ~__reallocation_buffer() {
            clear();
            if (__begin_) { alloc_traits::deallocate(__alloc_, __begin_, __cap_ - __begin_); }
        }

        _MYSTL_CONSTEXPR_SINCE_CXX20 void swap(__reallocation_buffer& __other) noexcept(!alloc_traits::propagate_on_container_swap::value) {
            std::swap(__begin_, __other.__begin_);
            std::swap(__end_, __other.__end_);
            std::swap(__cap_, __other.__cap_);
            std::swap(__alloc_, __other.__alloc_);
        }

        _MYSTL_CONSTEXPR_SINCE_CXX20 size_type size() const noexcept {
            return __end_ - __begin_;
        }

        _MYSTL_CONSTEXPR_SINCE_CXX20 void __construct_at(pointer __p, size_type __n) {
            pointer __new_p = __p + __n;
            for (; __p != __new_p; ++__p) { alloc_traits::construct(__alloc_, __p); }
        }

        _MYSTL_CONSTEXPR_SINCE_CXX20 void __construct_at(pointer __p, size_type __n, const_reference __x) {
            pointer __new_p = __p + __n;
            for (; __p != __new_p; ++__p) { alloc_traits::construct(__alloc_, __p, __x); }
        }

        template <class... _Args>
        _MYSTL_CONSTEXPR_SINCE_CXX20 void __construct_at(pointer __p, _Args&&... __args) {
            alloc_traits::construct(__alloc_, __p, std::forward<_Args>(__args)...);
        }

        template <class _InputIterator>
        _MYSTL_CONSTEXPR_SINCE_CXX20 void __construct_at(pointer __p, _InputIterator __first, size_type __n) {
            pointer __new_p = __p + __n;
            for (; __p != __new_p; ++__p, ++__first) { alloc_traits::construct(__alloc_, __p, std::move(*__first)); }
        }

        template <class _InputIterator, class _Sentinel>
        _MYSTL_CONSTEXPR_SINCE_CXX20 void __construct_at_end_with_sentinel(_InputIterator __first, _Sentinel __last) {
            for (; __first != __last; ++__first) {
                if (__end_ == __cap_) {
                    size_type __old_cap = __cap_ - __begin_;
                    size_type __new_cap = std::max<size_type>(2 * __old_cap, 8);
                    __reallocation_buffer __buffer(__alloc_, __new_cap);
                    for (pointer __p = __begin_; __p != __end_; ++__p, ++__buffer.__end_) {
                        alloc_traits::construct(__buffer.__alloc_, std::addressof(*__buffer.__end_), std::move(*__p));
                    }
                    swap(__buffer);
                }
                alloc_traits::construct(__alloc_, std::addressof(*__end_), *__first);
                ++__end_;
            }
        }
    };

    // 交换缓冲区与 vector 的数据
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __swap_reallocation_buffer(__reallocation_buffer& __buffer) {
        __uninitialized_allocator_relocate(__alloc_, __begin_, __end_, __buffer.__begin_);
        __buffer.__end_ += size();
        std::swap(__begin_, __buffer.__begin_);
        std::swap(__end_, __buffer.__end_);
        std::swap(__cap_, __buffer.__cap_);
        __buffer.__end_ = __buffer.__begin_; // 在 __uninitialized_allocator_relocate() 中已经将元素析构了
    }

    // 将 [__begin, __p) 和 [__p, __end_) 的数据交换到缓冲区
    // 的 [__new_begin, __new_p] 和 [__new_p + __n, __new_end_)
    // 缓冲区的 size() 会被设置为 vector.size() + __n
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __swap_reallocation_buffer(__reallocation_buffer& __buffer, pointer __p, size_type __n) {
        pointer _new_p = __buffer.__begin_ + (__p - __begin_);
        // 先交换 [__p, __end_) 到 [__new_p + __n, __new_end_)
        __uninitialized_allocator_relocate(__alloc_, __p, __end_, _new_p + __n);
        // 再交换 [__begin_, __p) 到 [__new_begin, __new_p]
        __uninitialized_allocator_relocate(__alloc_, __begin_, __p, __buffer.__begin_);
        __buffer.__end_ += size() + __n;
        std::swap(__begin_, __buffer.__begin_);
        std::swap(__end_, __buffer.__end_);
        std::swap(__cap_, __buffer.__cap_);
        __buffer.__end_ = __buffer.__begin_;
    }

    // 触发扩容时的 emplace_back()
    // Precondition: size() == capacity()
    // Postcondition: size() = old size() + 1
    // Postcondition: capacity() = __recommend(old size() + 1)
    template <class... _Args>
    _MYSTL_CONSTEXPR_SINCE_CXX20 pointer __emplace_back_slow_path(_Args&&... __args) {
        // 计算新容量
        size_type __new_cap = __recommend(size() + 1);
        // 创建缓冲区
        __reallocation_buffer __buffer(__alloc_, __new_cap);
        // 在缓冲区末尾构造新的元素
        alloc_traits::construct(__alloc_, std::addressof(*__buffer.__end_), std::forward<_Args>(__args)...);
        ++__buffer.__end_;
        // 将旧数据移动到新缓冲区
        __swap_reallocation_buffer(__buffer);
        return __end_;
    }

    // 将 vector 中元素从末尾开始析构，一直到 __new_last处
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __base_destruct_at_end(pointer __new_last) noexcept {
        if constexpr (!std::is_trivially_destructible_v<value_type>) {
            pointer __soon_to_be_end = __end_;
            while (__new_last != __soon_to_be_end) { alloc_traits::destroy(__alloc_, std::addressof(*--__soon_to_be_end)); }
        }
        __end_ = __new_last;
    }

    template <class _InputIterator, class _Sentinel>
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __init_with_sentinel(_InputIterator __first, _Sentinel __last) {
        auto __guard = __make_exception_guard(__destroy_vector(*this));
        for (; __first != __last; ++__first) { emplace_back(*__first); }
        __guard.__complete();
    }

    template <class _ForwardIterator, class _Sentinel>
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __init_with_size(_ForwardIterator __first, _Sentinel __last, size_type __n) {
        auto __guard = __make_exception_guard(__destroy_vector(*this));
        if (__n > 0) {
            __vallocate(__n);
            __construct_at_end(__first, __last, __n);
        }

        __guard.__complete();
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 iterator __make_iter(pointer __p) noexcept {
        return iterator(__p);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 const_iterator __make_iter(const_pointer __p) const noexcept {
        return const_iterator(__p);
    }

    // 将 [__from_s, __from_e) 之间的元素移动到 __to 开始的位置
    // 要求 __to - __from_s >= 0
    // 要求 vector 有足够的空间，即 capacity() >= size() + __to - __from_s
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __move_range(pointer __from_s, pointer __from_e, pointer __to) {
        pointer __old_last  = __end_;
        difference_type __n = __end_ - __to; // 需要移动的元素数量，其余元素需要重新构造
        {
            pointer __i = __from_s + __n; // __i 与 __from_e 之间的相对关系反映了原本在 __from_e 的元素移动后与 __end_ 的相对关系
            _ConstructTransaction __tx(*this, __from_e - __i); // __from_e - __i > 0 表示移动的区间超出了原本的范围，需要重新构造超出部分的元素
            for (; __i < __from_e; ++__i, ++__tx.__pos_) { alloc_traits::construct(__alloc_, std::addressof(*__tx.__pos_), std::move(*__i)); }
        }
        // 将剩余元素从后往前移动到指定位置
        std::move_backward(__from_s, __from_s + __n, __old_last);
    }

    template <class _InputIterator, class _Sentinel>
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __assign_with_sentinel(_InputIterator __first, _Sentinel __last) {
        pointer __current = __begin_;
        // 先在已有空间中赋值
        for (; __first != __last && __current != __end_; ++__first, ++__current) { *__current = *__first; }
        if (__current != __end_) { // 若现有空间足够，则将 vector 的 size 调整正确
            __base_destruct_at_end(__current);
        } else {                   // 继续构造元素
            for (; __first != __last; ++__first) { emplace_back(*__first); }
        }
    }

    template <class _ForwardIterator, class _Sentinel>
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __assign_with_size(_ForwardIterator __first, _Sentinel __last, difference_type __n) {
        size_type __new_size = static_cast<size_type>(__n);
        if (__new_size <= capacity()) {
            if (__new_size <= size()) {
                std::copy(std::move(__first), std::move(__last), __begin_);
                __base_destruct_at_end(__begin_ + __new_size);
            } else {
                _ForwardIterator __mid = std::next(__first, size());
                std::copy(std::move(__first), std::move(__mid), __begin_);
                __construct_at_end(std::move(__mid), std::move(__last), __new_size - size());
            }
        } else {
            __vdeallocate();
            __vallocate(__recommend(__new_size));
            __construct_at_end(std::move(__first), std::move(__last), __new_size);
        }
    }

    // 拷贝分配器，根据 propagate_on_container_copy_assignment 区分
    // 对于默认的分配器，为 false_type, 表示分配器不会随着容器的拷贝传播
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __copy_assign_alloc(const vector& __x) {
        __copy_assign_alloc(__x, std::integral_constant<bool, alloc_traits::propagate_on_container_copy_assignment::value>());
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void __copy_assign_alloc(const vector& __x, std::true_type) {
        if (__alloc_ != __x.__alloc_) { __vdeallocate(); }
        __alloc_ = __x.__alloc_;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void __copy_assign_alloc(const vector& __x, std::false_type) {}

    // 类似的，根据 propagate_on_container_move_assignment 区分
    // 默认为 true_type, 表示分配器会随着容器的移动赋值传播
    _MYSTL_CONSTEXPR_SINCE_CXX20 void __move_assign_alloc(vector& __x) noexcept(!alloc_traits::propagate_on_container_move_assignment::value ||
                                                                                std::is_nothrow_move_assignable<allocator_type>::value) {
        __move_assign_alloc(__x, std::integral_constant<bool, alloc_traits::propagate_on_container_move_assignment::value>());
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void __move_assign_alloc(vector& __x,
                                                          std::true_type) noexcept(std::is_nothrow_move_assignable<allocator_type>::value) {
        __alloc_ = std::move(__x.__alloc_);
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void __move_assign_alloc(vector& __x, std::false_type) noexcept {}

    _MYSTL_CONSTEXPR_SINCE_CXX20 void __move_assign(vector& __x, std::true_type) noexcept(std::is_nothrow_move_assignable<allocator_type>::type) {
        __vdeallocate();
        __move_assign_alloc(__x);
        __begin_     = __x.__begin_;
        __end_       = __x.__end_;
        __cap_       = __x.__cap_;
        __x.__begin_ = __x.__end_ = __x.__cap_ = nullptr;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 void __move__assign(vector& __x, std::false_type) noexcept(alloc_traits::is_always_equal::value) {
        if (__alloc_ != __x.__alloc_) {
            assign(__x.begin(), __x.end());
        } else {
            __move__assign(__x, std::true_type);
        }
    }
};

// CTAD
template <class _InputIterator, class _Alloc = allocator<typename iterator_traits<_InputIterator>::value_type>,
          typename = std::enable_if_t<is_based_on_input_iterator<_InputIterator>::value>, typename = std::enable_if_t<is_allocator<_Alloc>::value>>
vector(_InputIterator, _InputIterator) -> vector<typename iterator_traits<_InputIterator>::value_type, _Alloc>;

template <class _InputIterator, class _Alloc = allocator<typename iterator_traits<_InputIterator>::value_type>,
          typename = std::enable_if_t<is_based_on_input_iterator<_InputIterator>::value>, typename = std::enable_if_t<is_allocator<_Alloc>::value>>
vector(_InputIterator, _InputIterator, _Alloc) -> vector<typename iterator_traits<_InputIterator>::value_type, _Alloc>;

_MYSTL_END_NAMESPACE_MYSTL

#endif // _MYSTL_VECTOR_H
#ifndef _MYSTL_LIST_H
#define _MYSTL_LIST_H

#include <allocation_guard.h>
#include <allocator.h>
#include <config.h>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator.h>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

_MYSTL_BEGIN_NAMESPACE_MYSTL

// __list_node 为存储元素的节点
template <class _Tp, class _VoidPtr>
struct __list_node;
// __list_node_base 为根节点
template <class _Tp, class _VoidPtr>
struct __list_node_base;

// 将指针在不同类型之间转换
template <class _From, class _To>
using __rebind_pointer_t = typename std::pointer_traits<_From>::template rebind<_To>;

// 这样子定义的 __node_pointer 和 __base_pointer 可以适配不同的 _VoidPtr
// 从而可以使用原生指针，或是智能指针等等
template <class _Tp, class _VoidPtr>
struct __list_node_pointer_traits {
    using __node_pointer = __rebind_pointer_t<_VoidPtr, __list_node<_Tp, _VoidPtr>>;
    using __base_pointer = __rebind_pointer_t<_VoidPtr, __list_node_base<_Tp, _VoidPtr>>;
};

// 定义根节点类型
template <class _Tp, class _VoidPtr>
struct __list_node_base {
    using _NodeTraits    = __list_node_pointer_traits<_Tp, _VoidPtr>;
    using __node_pointer = typename _NodeTraits::__node_pointer;
    using __base_pointer = typename _NodeTraits::__base_pointer;

    __base_pointer __prev_;
    __base_pointer __next_;

    __list_node_base() : __prev_(__self()), __next_(__self()) {}

    explicit __list_node_base(__base_pointer __prev, __base_pointer __next) : __prev_(__prev), __next_(__next) {}

    __base_pointer __self() { return std::pointer_traits<__base_pointer>::pointer_to(*this); }

    __node_pointer __as_node() { return static_cast<__node_pointer>(__self()); }
};

// 定义内部存储数据的节点类型
template <class _Tp, class _VoidPtr>
struct __list_node : public __list_node_base<_Tp, _VoidPtr> {
public:
    using __base         = __list_node_base<_Tp, _VoidPtr>;
    using __base_pointer = typename __base::__base_pointer;

private:
    // 使用 union 延迟初始化
    union {
        _Tp __value_;
    };

public:
    _Tp& __get_value() { return __value_; }

    explicit __list_node(__base_pointer __prev, __base_pointer __next) : __base(__prev, __next) {}

    ~__list_node() {}

    __base_pointer __as_link() { return __base::__self(); }
};

// 前置声明，解决声明中的互相依赖关系
template <class _Tp, class _Alloc = allocator<_Tp>>
class list;
template <class _Tp, class _Alloc>
class __list_imp;
template <class _Tp, class _VoidPtr>
class __list_iterator;
template <class _Tp, class _VoidPtr>
class __list_const_iterator;

// list 的迭代器实现
template <class _Tp, class _VoidPtr>
class __list_iterator {
public:
    using value_type        = _Tp;
    using reference         = value_type&;
    using pointer           = __rebind_pointer_t<_VoidPtr, value_type>;
    using difference_type   = typename std::pointer_traits<pointer>::difference_type;
    using iterator_category = bidirectional_iterator_tag;

private:
    using _NodeTraits    = __list_node_pointer_traits<_Tp, _VoidPtr>;
    using __base_pointer = typename _NodeTraits::__base_pointer;

    __base_pointer __ptr_;

    explicit __list_iterator(__base_pointer __p) noexcept : __ptr_(__p) {}

    template <class, class>
    friend class list;
    template <class, class>
    friend class __list_imp;
    template <class, class>
    friend class __list_const_iterator;

public:
    __list_iterator() noexcept : __ptr_(nullptr) {}

    reference operator*() const { return __ptr_->__as_node()->__get_value(); }

    pointer operator->() const { return std::pointer_traits<pointer>::pointer_to(__ptr_->__as_node()->__get_value()); }

    __list_iterator& operator++() {
        __ptr_ = __ptr_->__next_;
        return *this;
    }

    __list_iterator operator++(int) {
        __list_iterator __tmp(*this);
        ++(*this);
        return __tmp;
    }

    __list_iterator& operator--() {
        __ptr_ = __ptr_->__prev_;
        return *this;
    }

    __list_iterator operator--(int) {
        __list_iterator __tmp(*this);
        --(*this);
        return __tmp;
    }

    __list_iterator operator+(difference_type __n) const {
        __list_iterator __tmp = *this;
        while (__n-- > 0 && __tmp.__ptr_->__next_) { ++__tmp; }
        return __tmp;
    }

    __list_iterator& operator+=(difference_type __n) {
        while (__n-- > 0 && __ptr_->__next_) { ++(*this); }
        return *this;
    }

    __list_iterator operator-(difference_type __n) const {
        __list_iterator __tmp = *this;
        while (__n-- > 0 && __tmp.__ptr_->__prev_) { --__tmp; }
        return __tmp;
    }

    __list_iterator& operator-=(difference_type __n) {
        while (__n-- > 0 && __ptr_->__prev_) { --(*this); }
        return *this;
    }

    friend bool operator==(const __list_iterator& __x, const __list_iterator& __y) { return __x.__ptr_ == __y.__ptr_; }

    friend bool operator!=(const __list_iterator& __x, const __list_iterator& __y) { return !(__x == __y); }
};

template <class _Tp, class _VoidPtr>
class __list_const_iterator {
public:
    using value_type        = _Tp;
    using reference         = const value_type&;
    using pointer           = __rebind_pointer_t<_VoidPtr, const value_type>;
    using difference_type   = typename std::pointer_traits<pointer>::difference_type;
    using iterator_category = bidirectional_iterator_tag;

private:
    using _NodeTraits    = __list_node_pointer_traits<_Tp, _VoidPtr>;
    using __base_pointer = typename _NodeTraits::__base_pointer;

    __base_pointer __ptr_;

    explicit __list_const_iterator(__base_pointer __p) noexcept : __ptr_(__p) {}

    template <class, class>
    friend class list;
    template <class, class>
    friend class __list_imp;

public:
    __list_const_iterator() noexcept : __ptr_(nullptr) {}

    __list_const_iterator(const __list_iterator<_Tp, _VoidPtr>& __p) noexcept : __ptr_(__p.__ptr_) {}

    reference operator*() const { return __ptr_->__as_node()->__get_value(); }

    pointer operator->() const { return std::pointer_traits<pointer>::pointer_to(__ptr_->__as_node()->__get_value()); }

    __list_const_iterator& operator++() {
        __ptr_ = __ptr_->__next_;
        return *this;
    }

    __list_const_iterator operator++(int) {
        __list_const_iterator __tmp(*this);
        ++(*this);
        return __tmp;
    }

    __list_const_iterator& operator--() {
        __ptr_ = __ptr_->__prev_;
        return *this;
    }

    __list_const_iterator operator--(int) {
        __list_const_iterator __tmp(*this);
        --(*this);
        return __tmp;
    }

    __list_const_iterator operator+(difference_type __n) const {
        __list_const_iterator __tmp = *this;
        while (__n-- > 0 && __tmp.__ptr_->__next_) { ++__tmp; }
        return __tmp;
    }

    __list_const_iterator& operator+=(difference_type __n) {
        while (__n-- > 0 && __ptr_->__next_) { ++(*this); }
        return *this;
    }

    __list_const_iterator operator-(difference_type __n) const {
        __list_const_iterator __tmp = *this;
        while (__n-- > 0 && __tmp.__ptr_->__prev_) { --__tmp; }
        return __tmp;
    }

    __list_const_iterator& operator-=(difference_type __n) {
        while (__n-- > 0 && __ptr_->__prev_) { --(*this); }
        return *this;
    }

    friend bool operator==(const __list_const_iterator& __x, const __list_const_iterator& __y) { return __x.__ptr_ == __y.__ptr_; }

    friend bool operator!=(const __list_const_iterator& __x, const __list_const_iterator& __y) { return !(__x == __y); }
};

// 循环双向链表
template <class _Tp, class _Alloc>
class __list_imp {
public:
    __list_imp(const __list_imp&)            = delete;
    __list_imp& operator=(const __list_imp&) = delete;

    using allocator_type = _Alloc;
    using alloc_traits   = std::allocator_traits<_Alloc>;
    using size_type      = typename alloc_traits::size_type;

protected:
    using value_type               = _Tp;
    using __void_pointer           = typename alloc_traits::void_pointer;
    using iterator                 = __list_iterator<value_type, __void_pointer>;
    using const_iterator           = __list_const_iterator<value_type, __void_pointer>;
    using __node_base              = __list_node_base<value_type, __void_pointer>;
    using __node_type              = __list_node<value_type, __void_pointer>;
    using __node_allocator         = alloc_traits::template rebind_alloc<__node_type>;
    using __node_alloc_traits      = std::allocator_traits<__node_allocator>;
    using __node_pointer           = typename __node_alloc_traits::pointer;
    using __node_const_pointer     = typename __node_alloc_traits::pointer;
    using __node_pointer_traits    = __list_node_pointer_traits<value_type, __void_pointer>;
    using __base_pointer           = typename __node_pointer_traits::__base_pointer;
    using __link_const_pointer     = __base_pointer;
    using pointer                  = typename alloc_traits::pointer;
    using const_pointer            = typename alloc_traits::const_pointer;
    using difference_type          = typename alloc_traits::difference_type;
    using __node_base_allocator    = alloc_traits::template rebind_alloc<__node_base>;
    using __node_base_alloc_traits = std::allocator_traits<__node_base_allocator>;
    using __node_base_pointer      = typename __node_base_alloc_traits::pointer;

    static_assert(!std::is_same<allocator_type, __node_allocator>::value, "internal allocator type must differ from user-specified type; "
                                                                          "otherwise overload resolution breaks");

    // 当链表为空时, __end_ 的前后都指向自己; 当非空时, __end_.__next_ 指向第一个有效节点, __end_.__prev_ 指向最后一个有效节点
    __node_base __end_;
    size_type __size_;
    __node_allocator __node_alloc_;

    __list_imp() noexcept(std::is_nothrow_default_constructible<__node_allocator>::value) : __size_(0) {}

    __list_imp(const allocator_type& __a) : __size_(0), __node_alloc_(__node_allocator(__a)) {}

    __list_imp(const __node_allocator& __a) : __size_(0), __node_alloc_(__a) {}

    __list_imp(__node_allocator&& __a) : __size_(0), __node_alloc_(std::move(__a)) {}

    ~__list_imp() { clear(); }

    void clear() noexcept {
        if (!empty()) {
            __base_pointer __f = __end_.__next_;
            __base_pointer __l = __end_as_link();
            __unlink_nodes(__f, __l->__prev_);
            __size_ = 0;
            while (__f != __l) {
                __node_pointer __np = __f->__as_node();
                __f                 = __f->__next_;
                __delete_node(__np);
            }
        }
    }

    bool empty() const noexcept { return __size_ == 0; };

    iterator begin() noexcept { return iterator(__end_.__next_); }

    const_iterator begin() const noexcept { return const_iterator(__end_.__next_); }

    iterator end() noexcept { return iterator(__end_as_link()); }

    const_iterator end() const noexcept { return const_iterator(__end_as_link()); }

    void swap(__list_imp& __other) noexcept {
        static_assert(alloc_traits::propagate_on_container_swap::value || this->__node_alloc_ == __other.__node_alloc_,
                      "list::swap: Either propagate_on_container_swap must be true"
                      " or the allocators must compare equal");
        std::swap(__node_alloc_, __other.__node_alloc_);
        std::swap(__size_, __other.__size_);
        std::swap(__end_, __other.__end_);
        if (__size_ == 0) {
            __end_.__next_ = __end_.__prev_ = __end_as_link();
        } else {
            __end_.__prev_->__next_ = __end_.__next_->__prev_ = __end_as_link(); // 将内部节点指向正确的 __end_
        }
        if (__other.__size_ == 0) {
            __other.__end_.__next_ = __other.__end_.__prev_ = __other.__end_as_link();
        } else {
            __other.__end_.__prev_->__next_ = __other.__end_.__next_->__prev_ = __other.__end_as_link(); // 将内部节点指向正确的 __end_
        }
    }

    // 返回非 const 指向 __end_ 的指针
    __base_pointer __end_as_link() const noexcept { return const_cast<__node_base&>(__end_).__self(); }

    size_type __node_alloc_max_size() const noexcept { return __node_alloc_traits::max_size(__node_alloc_); }

    // 创建节点
    template <class... _Args>
    __node_pointer __create_node(__base_pointer __prev, __base_pointer __next, _Args&&... __args) {
        __allocation_guard<__node_allocator> __guard(__node_alloc_, 1);

// 不使用 allocator 提供的 construct 函数，因此不需要要求 allocator 提供 __node_type 的 construct() 方法
// libc++ 中使用内置的 __construct_at 函数，这里模仿实现
#if _MYSTL_CXX_VERSION >= 20
        std::construct_at(std::addressof(*__guard.__get()), __prev, __next);
#else
        ::new (static_cast<void*>(std::addressof(*__guard.__get()))) __node_type(__prev, __next);
#endif

        // 在创建好的节点上创建指定的 value
        __node_alloc_traits::construct(__node_alloc_, std::addressof(__guard.__get()->__get_value()), std::forward<_Args>(__args)...);
        return __guard.__release_ptr();
    }

    // 删除节点
    void __delete_node(__node_pointer __node) {
        // 同样的，不使用 allocator 的 destroy()
        __node_alloc_traits::destroy(__node_alloc_, std::addressof(__node->__get_value()));
#if _MYSTL_CXX_VERSION >= 20
        std::destroy_at(std::addressof(*__node));
#else
        __node->~__node_type();
#endif
        __node_alloc_traits::deallocate(__node_alloc_, __node, 1);
    }

    void __copy_assign_alloc(const __list_imp& __other) {
        __copy_assign_alloc(__other, std::integral_constant<bool, __node_alloc_traits::propagate_on_container_copy_assignment::value>());
    }

    void __move_assign_alloc(__list_imp& __other) noexcept(!__node_alloc_traits::propagate_on_container_copy_assignment::value ||
                                                           std::is_nothrow_move_assignable<__node_allocator>::value) {
        __move_assign_alloc(__other, std::integral_constant<bool, __node_alloc_traits::propagate_on_container_move_assignment::value>());
    }

    // 将 [__first, __last] 之间的节点从链表中断开
    void __unlink_nodes(__base_pointer __first, __base_pointer __last) noexcept {
        __first->__prev_->__next_ = __last->__next_;
        __last->__next_->__prev_  = __first->__prev_;
    }

private:
    void __copy_assign_alloc(const __list_imp& __other, std::true_type) {
        if (__node_alloc_ != __other.__node_alloc_) { clear(); }
        __node_alloc_ = __other.__node_alloc_;
    }

    void __copy_assign_alloc(const __list_imp& __other, std::false_type) {}

    void __move_assign_alloc(const __list_imp& __other, std::true_type) noexcept(std::is_nothrow_move_assignable<__node_allocator>::value) {
        __node_alloc_ = std::move(__other.__node_alloc_);
    }

    void __move_assign_alloc(const __list_imp& __other, std::false_type) noexcept {}
};

template <class _Tp, class _Alloc /*= mystl::allocator<_Tp>*/>
class list : private __list_imp<_Tp, _Alloc> {
    using __base              = __list_imp<_Tp, _Alloc>;
    using __node_type         = typename __base::__node_type;
    using __node_allocator    = typename __base::__node_allocator;
    using __node_pointer      = typename __base::__node_pointer;
    using __node_alloc_traits = typename __base::__node_alloc_traits;
    using __node_base         = typename __base::__node_base;
    using __node_base_pointer = typename __base::__node_base_pointer;
    using __base_pointer      = typename __base::__base_pointer;

public:
    using value_type             = _Tp;
    using allocator_type         = _Alloc;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = typename __base::pointer;
    using const_pointer          = typename __base::const_pointer;
    using size_type              = typename __base::size_type;
    using difference_type        = typename __base::difference_type;
    using iterator               = typename __base::iterator;
    using const_iterator         = typename __base::const_iterator;
    using reverse_iterator       = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    //
    // constuct, destroy
    //
    list() noexcept(std::is_nothrow_default_constructible<__node_allocator>::value) {}

    explicit list(const allocator_type& __a) : __base(__a) {}

    explicit list(size_type __n) {
        for (; __n > 0; --__n) { emplace_back(); }
    }

    explicit list(size_type __n, const allocator_type& __a) : __base(__a) {
        for (; __n > 0; --__n) { emplace_back(); }
    }

    list(size_type __n, const value_type& __x) {
        for (; __n > 0; --__n) { emplace_back(__x); }
    }

    list(size_type __n, const value_type& __x, const allocator_type& __a) : __base(__a) {
        for (; __n > 0; --__n) { emplace_back(__x); }
    }

#if _MYSTL_CXX_VERSION <= 17
    template <class _InputIterator, std::enable_if_t<mystl::is_based_on_input_iterator<_InputIterator>::value>>
#else
    template <BasedOnInputIterator _InputIterator>
#endif
    list(_InputIterator __first, _InputIterator __last) {
        for (; __first != __last; ++__first) { emplace_back(*__first); }
    }

#if _MYSTL_CXX_VERSION <= 17
    template <class _InputIterator, std::enable_if_t<mystl::is_based_on_input_iterator<_InputIterator>::value>>
#else
    template <BasedOnInputIterator _InputIterator>
#endif
    list(_InputIterator __first, _InputIterator __last, const allocator_type& __a) : __base(__a) {
        for (; __first != __last; ++__first) { emplace_back(*__first); }
    }

    list(const list& __other) : __base(__node_alloc_traits::select_on_container_copy_construction(__other.__node_alloc_)) {
        for (const_iterator __f = __other.begin(), __l = __other.end(); __f != __l; ++__f) { emplace_back(*__f); }
    }

    list(const list& __other, const allocator_type& __a) : __base(__a) {
        for (const_iterator __f = __other.begin(), __l = __other.end(); __f != __l; ++__f) { emplace_back(*__f); }
    }

    list& operator=(const list& __other) {
        if (this != std::addressof(__other)) {
            __base::__copy_assign_alloc(__other);
            assign(__other.begin(), __other.end());
        }
        return *this;
    }

    list(std::initializer_list<value_type> __il) {
        for (typename std::initializer_list<value_type>::const_iterator __f = __il.begin(), __l = __il.end(); __f != __l; ++__f) {
            emplace_back(*__f);
        }
    }

    list(std::initializer_list<value_type> __il, const allocator_type& __a) : __base(__a) {
        for (typename std::initializer_list<value_type>::const_iterator __f = __il.begin(), __l = __il.end(); __f != __l; ++__f) {
            emplace_back(*__f);
        }
    }

    list(list&& __other) noexcept(std::is_nothrow_move_constructible<allocator_type>::value) : __base(std::move(__other.__node_alloc_)) {
        splice(end(), __other);
    }

    list(list&& __other, const allocator_type& __a) {
        if (__a == __other.__node_alloc_) {
            splice(end(), __other);
        } else {
            assign(__other.begin(), __other.end());
        }
    }

    list& operator=(list&& __other) noexcept((__node_alloc_traits::propagate_on_container_move_assignment::value &&
                                              std::is_nothrow_move_assignable<__node_allocator>::value) ||
                                             std::allocator_traits<allocator_type>::is_always_equal::value) {
        __move_assign(__other, std::integral_constant<bool, __node_alloc_traits::propagate_on_container_move_assignment::value>());
    }

#if _MYSTL_CXX_VERSION <= 17
    template <class _InputIterator, std::enable_if_t<mystl::is_based_on_input_iterator<_InputIterator>::value>>
#else
    template <BasedOnInputIterator _InputIterator>
#endif
    void assign(_InputIterator __first, _InputIterator __last) {
        iterator __i = begin();
        iterator __e = end();
        // 先在已有节点上赋值
        for (; __first != __last && __i != __e; ++__first, ++__i) { *__i = *__first; }

        if (__i == __e) {
            // 若还有元素，则视为在末尾插入剩余节点
            insert(__e, std::move(__first), std::move(__last));
        } else {
            // 若已经完成，删除多余节点
            erase(__i, __e);
        }
    }

    void assign(std::initializer_list<value_type> __il) { assign(__il.begin(), __il.end()); }

    void assign(size_type __n, const value_type& __x) {
        iterator __i = begin();
        iterator __e = end();
        // 先在已有节点上赋值
        for (; __n > 0 && __i != __e; --__n, ++__i) { *__i = __x; }

        if (__i == __e) {
            // 若还有元素，则视为在末尾插入剩余节点
            insert(__e, __n, __x);
        } else {
            // 若已经完成，删除多余节点
            erase(__i, __e);
        }
    }

    allocator_type get_allocator() const noexcept { return allocator_type(this->__node_alloc_); }

    size_type size() const noexcept { return this->__size_; }

    [[nodiscard]] bool empty() const noexcept { return __base::empty(); }

    size_type max_size() const noexcept { return std::min<size_type>(this->__node_alloc_max_size(), std::numeric_limits<size_type>::max()); }

    //
    // iterators
    //
    iterator begin() noexcept { return __base::begin(); }

    const_iterator begin() const noexcept { return __base::begin(); }

    iterator end() noexcept { return __base::end(); }

    const_iterator end() const noexcept { return __base::end(); }

    const_iterator cbegin() const noexcept { return __base::begin(); }

    const_iterator cend() const noexcept { return __base::end(); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }

    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

    reference front() { return __base::__end_.__next_->__as_node()->__get_value(); }

    const_reference front() const { return __base::__end_.__next_->__as_node()->__get_value(); }

    reference back() { return __base::__end_.__prev_->__as_node()->__get_value(); }

    const_reference back() const { return __base::__end_.__prev_->__as_node()->__get_value(); }

    //
    // modifiers
    //
    void push_front(const value_type& __x) { emplace_front(__x); }

    void push_front(value_type&& __x) { emplace_front(std::move(__x)); }

    void push_back(const value_type& __x) { emplace_back(__x); }

    void push_back(value_type&& __x) { emplace_back(std::move(__x)); }

    template <class... _Args>
    reference emplace_front(_Args&&... __args) {
        __node_pointer __node = __base::__create_node(nullptr, nullptr, std::forward<_Args>(__args)...);
        __base_pointer __nl   = __node->__as_link();
        __link_nodes_at_front(__nl, __nl);
        ++__base::__size_;
        return __node->__get_value();
    }

    template <class... _Args>
    reference emplace_back(_Args&&... __args) {
        __node_pointer __node = __base::__create_node(nullptr, nullptr, std::forward<_Args>(__args)...);
        __base_pointer __nl   = __node->__as_link();
        __link_nodes_at_back(__nl, __nl);
        ++__base::__size_;
        return __node->__get_value();
    }

    template <class... _Args>
    iterator emplace(const_iterator __p, _Args&&... __args) {
        __node_pointer __node = __base::__create_node(nullptr, nullptr, std::forward<_Args>(__args)...);
        __base_pointer __nl   = __node->__as_link();
        __link_nodes(__p.__ptr_, __nl, __nl);
        ++__base::__size_;
        return iterator(__nl);
    }

    iterator insert(const_iterator __p, value_type&& __x) { return emplace(__p, std::move(__x)); }

    iterator insert(const_iterator __p, const value_type& __x) { return emplace(__p, __x); }

    iterator insert(const_iterator __p, size_type __n, const value_type& __x) {
        iterator __r(__p.__ptr_);
        if (__n > 0) {
            size_type __ds        = 0;
            __node_pointer __node = __base::__create_node(nullptr, nullptr, __x);
            ++__ds;
            __r          = iterator(__node->__as_link());
            iterator __e = __r;
#if _MYSTL_HAS_EXCEPTIONS
            try {
#endif
                for (--__n; __n != 0; --__n, ++__e, ++__ds) { __e.__ptr_->__next_ = __base::__create_node(__e.__ptr_, nullptr, __x)->__as_link(); }
#if _MYSTL_HAS_EXCEPTIONS
            } catch (...) {
                while (true) {
                    __base_pointer __prev    = __e.__ptr_->__prev_;
                    __node_pointer __current = __e.__ptr_->__as_node();
                    __base::__delete_node(__current);
                    if (__prev == nullptr) { break; }
                    __e = iterator(__prev);
                }
                throw;
            }
#endif // _MYSTL_HAS_EXCEPTIONS
            __link_nodes(__p.__ptr_, __r.__ptr_, __e.__ptr_);
            __base::__size_ += __ds;
        }
        return __r;
    }

#if _MYSTL_CXX_VERSION <= 17
    template <class _InputIterator, std::enable_if_t<mystl::is_based_on_input_iterator<_InputIterator>::value>>
#else
    template <BasedOnInputIterator _InputIterator>
#endif
    iterator insert(const_iterator __p, _InputIterator __first, _InputIterator __last) {
        iterator __r(__p.__ptr_);
        if (__first != __last) {
            size_type __ds        = 0;
            __node_pointer __node = __base::__create_node(nullptr, nullptr, *__first);
            ++__ds;
            __r          = iterator(__node->__as_link());
            iterator __e = __r;
#if _MYSTL_HAS_EXCEPTIONS
            try {
#endif
                for (++__first; __first != __last; ++__first, ++__e, ++__ds) {
                    __e.__ptr_->__next_ = __base::__create_node(__e.__ptr_, nullptr, *__first)->__as_link();
                }
#if _MYSTL_HAS_EXCEPTIONS
            } catch (...) {
                while (true) {
                    __base_pointer __prev    = __e.__ptr_->__prev_;
                    __node_pointer __current = __e.__ptr_->__as_node();
                    __base::__delete_node(__current);
                    if (__prev == nullptr) { break; }
                    __e = iterator(__prev);
                }
                throw;
            }
#endif // _MYSTL_HAS_EXCEPTIONS
            __link_nodes(__p.__ptr_, __r.__ptr_, __e.__ptr_);
            __base::__size_ += __ds;
        }
        return __r;
    }

    iterator insert(const_iterator __p, std::initializer_list<value_type> __il) { return insert(__p, __il.begin(), __il.end()); }

    void swap(list& __other) noexcept { __base::swap(__other); }

    void clear() noexcept { __base::clear(); }

    void pop_front() {
        __base_pointer __n = __base::__end_.__next_;
        __base::__unlink_nodes(__n, __n);
        --__base::__size_;
        __base::__delete_node(__n->__as_node());
    }

    void pop_back() {
        __base_pointer __n = __base::__end_.__prev_;
        __base::__unlink_nodes(__n, __n);
        --__base::__size_;
        __base::__delete_node(__n->__as_node());
    }

    iterator erase(const_iterator __p) {
        __base_pointer __n = __p.__ptr_;
        __base_pointer __r = __n->__next_;
        __base::__unlink_nodes(__n, __n);
        --__base::__size_;
        __base::__delete_node(__n->__as_node());
        return iterator(__r);
    }

    // 删除 [__first, __last) 之间的节点
    iterator erase(const_iterator __first, const_iterator __last) {
        if (__first != __last) {
            __base::__unlink_nodes(__first.__ptr_, __last.__ptr_->__prev_);
            while (__first != __last) {
                __base_pointer __n = __first.__ptr_;
                ++__first;
                --__base::__size_;
                __base::__delete_node(__n->__as_node());
            }
        }
        return iterator(__last.__ptr_);
    }

    void resize(size_type __n) {
        if (__n < __base::__size_) {
            erase(__iterator(__n), end());
        } else if (__n > __base::__size_) {
            __n -= __base::__size_;
            size_type __ds        = 0;
            __node_pointer __node = __base::__create_node(nullptr, nullptr);
            ++__ds;
            iterator __r = iterator(__node->__as_link());
            iterator __e = __r;
#if _MYSTL_HAS_EXCEPTIONS
            try {
#endif
                for (--__n; __n != 0; --__n, ++__e, ++__ds) { __e.__ptr_->__next_ = __base::__create_node(__e.__ptr_, nullptr)->__as_link(); }
#if _MYSTL_HAS_EXCEPTIONS
            } catch (...) {
                while (true) {
                    __base_pointer __prev    = __e.__ptr_->__prev_;
                    __node_pointer __current = __e.__ptr_->__as_node();
                    __base::__delete_node(__current);
                    if (__prev == nullptr) { break; }
                    __e = iterator(__prev);
                }
                throw;
            }
#endif
            __link_nodes_at_back(__r.__ptr_, __e.__ptr_);
            __base::__size_ += __ds;
        }
    }

    void resize(size_type __n, const value_type& __x) {
        if (__n < __base::__size_) {
            erase(__iterator(__n), end());
        } else if (__n > __base::__size_) {
            __n -= __base::__size_;
            size_type __ds        = 0;
            __node_pointer __node = __base::__create_node(nullptr, nullptr, __x);
            ++__ds;
            iterator __r = iterator(__node->__as_link());
            iterator __e = __r;
#if _MYSTL_HAS_EXCEPTIONS
            try {
#endif
                for (--__n; __n != 0; --__n, ++__e, ++__ds) { __e.__ptr_->__next_ = __base::__create_node(__e.__ptr_, nullptr, __x)->__as_link(); }
#if _MYSTL_HAS_EXCEPTIONS
            } catch (...) {
                while (true) {
                    __base_pointer __prev    = __e.__ptr_->__prev_;
                    __node_pointer __current = __e.__ptr_->__as_node();
                    __base::__delete_node(__current);
                    if (__prev == nullptr) { break; }
                    __e = iterator(__prev);
                }
                throw;
            }
#endif
            __link_nodes_at_back(__r.__ptr_, __e.__ptr_);
            __base::__size_ += __ds;
        }
    }

    // 将 __other 合并到 this 的 __p 指向的元素之前
    void splice(const_iterator __p, list& __other) {
        if (!__other.empty()) {
            __base_pointer __first = __other.__end_.__next_;
            __base_pointer __last  = __other.__end_.__prev_;
            __base::__unlink_nodes(__first, __last);
            __link_nodes(__p.__ptr_, __first, __last);
            __base::__size_ += __other.__size_;
            __other.__size_ = 0;
        }
    }

    void splice(const_iterator __p, list&& __other) { splice(__p, __other); }

    // 将 __i 指向的元素从 __other 移动到 this 的 __p 指向的元素之前
    void splice(const_iterator __p, list& __other, const_iterator __i) {
        // 需要判断 __p 是否与 __i 指向同一个元素或本就在 __i 指向的元素的下一个
        if (__p.__ptr_ != __i.__ptr_ && __p.__ptr_ != __i.__ptr_->__next_) {
            __base_pointer __r = __i.__ptr_;
            __base::__unlink_nodes(__r, __r);
            __link_nodes(__p.__ptr_, __r, __r);
            --__other.__size_;
            ++__base::__size_;
        }
    }

    void splice(const_iterator __p, list&& __other, const_iterator __i) { splice(__p, __other, __i); }

    // 将 [__first, __last) 中的节点移动到 this 的 __p 指向的元素之前
    void splice(const_iterator __p, list& __other, const_iterator __first, const_iterator __last) {
        if (__first != __last) {
            __base_pointer __f = __first.__ptr_;
            --__last;
            __base_pointer __l = __last.__ptr_;
            if (this != std::addressof(__other)) {
                size_type __s = std::distance(__first, __last) + 1;
                __other.__size_ -= __s;
                __base::__size_ += __s;
            }
            __base::__unlink_nodes(__f, __l);
            __link_nodes(__p.__ptr_, __f, __l);
        }
    }

    void splice(const_iterator __p, list&& __other, const_iterator __first, const_iterator __last) { splice(__p, __other, __first, __last); }

    size_type remove(const value_type& __x) {
        list<value_type, allocator_type> __deleted_nodes(get_allocator());
        for (iterator __i = begin(), __e = end(); __i != __e;) {
            if (*__i == __x) {
                iterator __j = std::next(__i);
                for (; __j != __e && *__j == __x; ++__j); // 找到下一个不同元素为止
                __deleted_nodes.splice(__deleted_nodes.end(), *this, __i, __j);
                __i = __j; // __j 已经不满足条件
                if (__i != __e) { ++__i; }
            } else {
                ++__i;
            }
        }
        return __deleted_nodes.size();
    }

    template <class _Pred>
    size_type remove_if(_Pred __pred) {
        list<value_type, allocator_type> __deleted_nodes(get_allocator());
        for (iterator __i = begin(), __e = end(); __i != __e;) {
            if (__pred(*__i)) {
                iterator __j = std::next(__i);
                for (; __j != __e && __pred(*__j); ++__j); // 找到下一个不同元素为止
                __deleted_nodes.splice(__deleted_nodes.end(), *this, __i, __j);
                __i = __j;
                if (__i != __e) { ++__i; }
            } else {
                ++__i;
            }
        }
        return __deleted_nodes.size();
    }

    // 删除连续的重复元素，对于不连续的不删除
    template <class _BinaryPred>
    size_type unique(_BinaryPred __binary_pred) {
        list<value_type, allocator_type> __deleted_nodes(get_allocator());
        for (iterator __i = begin(), __e = end(); __i != __e;) {
            iterator __j = std::next(__i);
            for (; __j != __e && __binary_pred(*__i, *__j); ++__j); // 找到下一个不同元素为止
            if (++__i != __j) {
                __deleted_nodes.splice(__deleted_nodes.end(), *this, __i, __j);
                __i = __j;
            }
        }
        return __deleted_nodes.size();
    }

    size_type unique() { return unique(std::equal_to()); }

    void merge(list& __other) { merge(__other, std::less()); }

    void merge(list&& __other) { merge(__other); }

    template <class _Comp>
    void merge(list& __other, _Comp __comp) {
        if (this != std::addressof(__other)) {
            iterator __f1 = begin();
            iterator __e1 = end();
            iterator __f2 = __other.begin();
            iterator __e2 = __other.end();
            while (__f1 != __e1 && __f2 != __e2) {
                // __comp(a, b) == true 表示 a 应该在 b 前面
                if (__comp(*__f2, *__f1)) {
                    size_type __ds = 1;
                    iterator __m2  = std::next(__f2);
                    for (; __m2 != __e2 && __comp(*__m2, *__f1); ++__m2, ++__ds); // 找出连续的需要合并的一段
                    __base_pointer __f = __f2.__ptr_;
                    __base_pointer __l = __m2.__ptr_->__prev_;
                    __base::__unlink_nodes(__f, __l);
                    __link_nodes(__f1.__ptr_, __f, __l);
                    __base::__size_ += __ds;
                    __other.__size_ -= __ds;
                    __f2 = __m2;
                    __f1 = iterator(__l->__next_);
                } else {
                    ++__f1;
                }
            }
            splice(__e1, __other);
        }
    }

    template <class _Comp>
    void merge(list&& __other, _Comp __comp) {
        merge(__other, __comp);
    }

    // 归并排序
    void sort() { sort(std::less()); }

    template <class _Comp>
    void sort(_Comp __comp) {
        __sort(begin(), end(), __base::__size_, __comp);
    }

    void reverse() noexcept;

private:
    // 将 [__first, __last] 之间的节点连接到 __p 之前
    void __link_nodes(__base_pointer __p, __base_pointer __first, __base_pointer __last) {
        __first->__prev_          = __p->__prev_;
        __last->__next_           = __p;
        __first->__prev_->__next_ = __first;
        __last->__next_->__prev_  = __last;
    }

    // 将 [__first, __last] 之间的节点连接到 list 开头
    void __link_nodes_at_front(__base_pointer __first, __base_pointer __last) {
        __first->__prev_          = __base::__end_as_link();
        __last->__next_           = __base::__end_.__next_;
        __first->__prev_->__next_ = __first;
        __last->__next_->__prev_  = __last;
    }

    // 将 [__first, __last] 之间的节点连接到 list 末尾
    void __link_nodes_at_back(__base_pointer __first, __base_pointer __last) {
        __first->__prev_          = __base::__end_.__prev_;
        __last->__next_           = __base::__end_as_link();
        __first->__prev_->__next_ = __first;
        __last->__next_->__prev_  = __last;
    }

    // 获取第 __n 个迭代器
    iterator __iterator(size_type __n) {
        if (__n <= __base::__size_ / 2) {
            return std::next(begin(), __n);
        } else {
            return std::prev(end(), __base::__size_ - __n);
        }
    }

    void __move_assign(list& __other, std::true_type) noexcept(std::is_nothrow_move_assignable<__node_allocator>::value) {
        clear();
        __base::__move_assign_alloc(__other);
        splice(end(), __other);
    }

    void __move_assign(list& __other, std::false_type) {
        if (__base::__node_alloc_ == __other.__node_alloc_) {
            __move_assign(__other, std::true_type());
        } else {
            assign(__other.begin(), __other.end());
        }
    }

    // 归并排序，将 [__f1, __e2) 内的 __n 个节点按照升序排序
    // 返回迭代器指向排序后的第一个元素
    template <class _Comp>
    iterator __sort(iterator __f1, iterator __e2, size_type __n, _Comp& __comp) {
        if (__n < 2) {
            return __f1;
        } else if (__n == 2) {
            if (__comp(*--__e2, *__f1)) {
                __base_pointer __f = __e2.__ptr_;
                __base::__unlink_nodes(__f, __f);
                __link_nodes(__f1.__ptr_, __f, __f);
                return __e2;
            }
            return __f1;
        } else {
            // 分割为两个区间分别排序
            size_type __n2 = __n / 2;
            iterator __e1  = std::next(__f1, __n2);
            iterator __r = __f1 = __sort(__f1, __e1, __n2, __comp);
            iterator __f2 = __e1 = __sort(__e1, __e2, __n - __n2, __comp);

            // 此时两个区间 [__f1, __e1) 和 [__f2, __e2) 均有序
            // 合并两个区间即可
            // 为了便于返回排序后指向第一个元素的迭代器，先对第一个元素进行合并
            if (__comp(*__f2, *__f1)) {
                iterator __m2 = std::next(__f2);
                for (; __m2 != __e2 && __comp(*__m2, *__f1); ++__m2); // 找出连续的需要合并的一段
                __base_pointer __f = __f2.__ptr_;
                __base_pointer __l = __m2.__ptr_->__prev_;
                __base::__unlink_nodes(__f, __l);
                __link_nodes(__f1.__ptr_, __f, __l);
                __r  = __f2;
                __f2 = __m2;
                __f1 = iterator(__l->__next_);
            } else {
                ++__f1;
            }
            while (__f1 != __e1 && __f2 != __e2) {
                if (__comp(*__f2, *__f1)) {
                    iterator __m2 = std::next(__f2);
                    for (; __m2 != __e2 && __comp(*__m2, *__f1); ++__m2); // 找出连续的需要合并的一段
                    __base_pointer __f = __f2.__ptr_;
                    __base_pointer __l = __m2.__ptr_->__prev_;
                    __base::__unlink_nodes(__f, __l);
                    __link_nodes(__f1.__ptr_, __f, __l);
                    __f2 = __m2;
                    __f1 = iterator(__l->__next_);
                } else {
                    ++__f1;
                }
            }
            return __r;
        }
    }
};

// CTAD
template <class _InputIterator, class _Alloc = allocator<typename iterator_traits<_InputIterator>::value_type>,
          typename = std::enable_if_t<mystl::is_based_on_input_iterator<_InputIterator>::value>,
          typename = std::enable_if_t<mystl::is_allocator<_Alloc>::value>>
list(_InputIterator, _InputIterator) -> list<typename iterator_traits<_InputIterator>::value_type, _Alloc>;

template <class _InputIterator, class _Alloc = allocator<typename iterator_traits<_InputIterator>::value_type>,
          typename = std::enable_if_t<mystl::is_based_on_input_iterator<_InputIterator>::value>,
          typename = std::enable_if_t<mystl::is_allocator<_Alloc>::value>>
list(_InputIterator, _InputIterator, _Alloc) -> list<typename iterator_traits<_InputIterator>::value_type, _Alloc>;

_MYSTL_END_NAMESPACE_MYSTL

#endif // _MYSTL_LIST_H
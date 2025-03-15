#ifndef _MYSTL_LIST_H
#define _MYSTL_LIST_H

#include <allocator.h>
#include <config.h>
#include <iterator.h>
#include <type_traits>


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

// 将分配器在不同类型之间转换
template <class _Traits, class _Tp>
using __rebind_alloc = typename _Traits::template rebind_alloc<_Tp>;

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

    __base_pointer __self() {
        return std::pointer_traits<__base_pointer>::pointer_to(*this);
    }

    __node_pointer __as_node() {
        return static_cast<__node_pointer>(__self());
    }
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
    _Tp& __get_value() {
        return __value_;
    }

    explicit __list_node(__base_pointer __prev, __base_pointer __next) : __base(__prev, __next) {}

    ~__list_node() {}

    __base_pointer __as_link() {
        return __base::__self();
    }
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

    reference operator*() const {
        return __ptr_->__as_node()->__get_value();
    }

    pointer operator->() const {
        return std::pointer_traits<pointer>::pointer_to(__ptr_->__as_node()->__get_value());
    }

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

    friend bool operator==(const __list_iterator& __x, const __list_iterator& __y) {
        return __x.__ptr_ == __y.__ptr_;
    }

    friend bool operator!=(const __list_iterator& __x, const __list_iterator& __y) {
        return !(__x == __y);
    }
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

    reference operator*() const {
        return __ptr_->__as_node()->__get_value();
    }

    pointer operator->() const {
        return std::pointer_traits<pointer>::pointer_to(__ptr_->__as_node()->__get_value());
    }

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

    friend bool operator==(const __list_const_iterator& __x, const __list_const_iterator& __y) {
        return __x.__ptr_ == __y.__ptr_;
    }

    friend bool operator!=(const __list_const_iterator& __x, const __list_const_iterator& __y) {
        return !(__x == __y);
    }
};

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
    using __node_allocator         = __rebind_alloc<alloc_traits, __node_type>;
    using __node_alloc_traits      = std::allocator_traits<__node_allocator>;
    using __node_pointer           = typename __node_alloc_traits::pointer;
    using __node_const_pointer     = typename __node_alloc_traits::pointer;
    using __node_pointer_traits    = __list_node_pointer_traits<value_type, __void_pointer>;
    using __base_pointer           = typename __node_pointer_traits::__base_pointer;
    using __link_const_pointer     = __base_pointer;
    using pointer                  = typename alloc_traits::pointer;
    using const_pointer            = typename alloc_traits::const_pointer;
    using difference_type          = typename alloc_traits::difference_type;
    using __node_base_allocator    = __rebind_alloc<alloc_traits, __node_base>;
    using __node_base_alloc_traits = std::allocator_traits<__node_base_allocator>;
    using __node_base_pointer      = typename __node_base_alloc_traits::pointer;

    static_assert(!std::is_same<allocator_type, __node_allocator>::value,
                  "internal allocator type must differ from user-specified type; otherwise overload resolution breaks");

    __node_base __end_;
    size_type __size_;
    __node_allocator __node_alloc_;
};

_MYSTL_END_NAMESPACE_MYSTL

#endif // _MYSTL_LIST_H
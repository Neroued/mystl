#ifndef _MYSTL_ITERATOR_H
#define _MYSTL_ITERATOR_H

#include <config.h>
#include <iterator>
#include <type_traits>
#if _MYSTL_CXX_VERSION >= 20
#    include <concepts>
#endif

_MYSTL_BEGIN_NAMESPACE_MYSTL

//===--------------------------------------------------------------------------===//
//===----------------------- iterator_tag 的实现--------------------------------===//
// struct input_iterator_tag {};
// struct output_iterator_tag {};
// struct forward_iterator_tag : public input_iterator_tag {};
// struct bidirectional_iterator_tag : public forward_iterator_tag {};
// struct random_access_iterator_tag : public bidirectional_iterator_tag {};
// #if _MYSTL_CXX_VERSION >= 20
// struct contiguous_iterator_tag : public random_access_iterator_tag {};
// #endif
//===--------------------------------------------------------------------------===//


//===----------------------------------===//
//===--------为了兼容性使用 std --------===//
using std::input_iterator_tag;
using std::output_iterator_tag;
using std::forward_iterator_tag;
using std::bidirectional_iterator_tag;
using std::random_access_iterator_tag;
#if _MYSTL_CXX_VERSION >= 20
using std::contiguous_iterator_tag;
#endif

// 使用 std::iterator_traits
using std::iterator_traits;

// 对于 C++17 定义用于判断迭代器类型方法
// 用于基础判断
template <class Iter, typename Category>
using is_based_on_iterator = std::is_base_of<Category, typename iterator_traits<Iter>::iterator_category>;
template <class Iter, typename Category>
using is_exact_iterator = std::is_same<Category, typename iterator_traits<Iter>::iterator_category>;

// 对于特定类型迭代器特化
template <class Iter>
using is_based_on_input_iterator = is_based_on_iterator<Iter, input_iterator_tag>;
template <class Iter>
using is_based_on_output_iterator = is_based_on_iterator<Iter, output_iterator_tag>;
template <class Iter>
using is_based_on_forward_iterator = is_based_on_iterator<Iter, forward_iterator_tag>;
template <class Iter>
using is_based_on_bidirectional_iterator = is_based_on_iterator<Iter, bidirectional_iterator_tag>;
template <class Iter>
using is_based_on_random_access_iterator = is_based_on_iterator<Iter, random_access_iterator_tag>;

template <class Iter>
using is_exact_input_iterator = is_exact_iterator<Iter, input_iterator_tag>;
template <class Iter>
using is_exact_output_iterator = is_exact_iterator<Iter, output_iterator_tag>;
template <class Iter>
using is_exact_forward_iterator = is_exact_iterator<Iter, forward_iterator_tag>;
template <class Iter>
using is_exact_bidirectional_iterator = is_exact_iterator<Iter, bidirectional_iterator_tag>;
template <class Iter>
using is_exact_random_access_iterator = is_exact_iterator<Iter, random_access_iterator_tag>;

// C++20 引入 concepts
#if _MYSTL_CXX_VERSION >= 20
template <class Iter, typename Category>
concept BasedOnIterator = std::derived_from<typename iterator_traits<Iter>::iterator_category, Category>;
template <class Iter, typename Category>
concept ExactIterator = std::same_as<typename iterator_traits<Iter>::iterator_category, Category>;

template <class Iter>
concept BasedOnInputIterator = BasedOnIterator<Iter, input_iterator_tag>;
template <class Iter>
concept BasedOnOutputIterator = BasedOnIterator<Iter, output_iterator_tag>;
template <class Iter>
concept BasedOnForwardIterator = BasedOnIterator<Iter, forward_iterator_tag>;
template <class Iter>
concept BasedOnBidirectionalIterator = BasedOnIterator<Iter, bidirectional_iterator_tag>;
template <class Iter>
concept BasedOnRandomAccessIterator = BasedOnIterator<Iter, random_access_iterator_tag>;
// 对于 contiguous，需要使用 iterator_concept 判断
template <class Iter>
concept BasedOnContiguousIterator = std::derived_from<typename std::iterator_traits<Iter>::iterator_concept, std::contiguous_iterator_tag>;

template <class Iter>
concept ExactInputIterator = ExactIterator<Iter, input_iterator_tag>;
template <class Iter>
concept ExactOutputIterator = ExactIterator<Iter, output_iterator_tag>;
template <class Iter>
concept ExactForwardIterator = ExactIterator<Iter, forward_iterator_tag>;
template <class Iter>
concept ExactBidirectionalIterator = ExactIterator<Iter, bidirectional_iterator_tag>;
template <class Iter>
concept ExactRandomAccessIterator = ExactIterator<Iter, random_access_iterator_tag>;
template <class Iter>
concept ExactContiguousIterator = std::same_as<typename std::iterator_traits<Iter>::iterator_concept, std::contiguous_iterator_tag>;
#endif
//===----------------------------------===//


//===----------------------------------------------------------===//
//===--------------实现一个 wrap_iter, 用于包装指针--------------===//
template <class Iter>
class wrap_iter {
public:
    using iterator_type     = Iter;
    using value_type        = typename iterator_traits<Iter>::value_type;
    using difference_type   = typename iterator_traits<Iter>::difference_type;
    using pointer           = typename iterator_traits<Iter>::pointer;
    using reference         = typename iterator_traits<Iter>::reference;
    using iterator_category = typename iterator_traits<Iter>::iterator_category;
#if _MYSTL_CXX_VERSION >= 20
    using iterator_concept = contiguous_iterator_tag; // llvm 直接固定为 contiguous_iterator_tag, 因为只用于包装指针
#endif

private:
    iterator_type it;

public:
    _MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter() noexcept : it() {}

    // 一个 SFINAE, 检查两个迭代器之间是否可以互相转化
    template <class OtherIter,
              typename = std::enable_if_t<std::is_convertible<const OtherIter&, Iter>::value &&
                                          std::is_convertible<typename std::iterator_traits<OtherIter>::reference, reference>::value>>
    _MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter(const wrap_iter<OtherIter>& other) noexcept : it(other.it) {}

    // operators
    // 解引用与 ->
    [[nodiscard]] _MYSTL_CONSTEXPR_SINCE_CXX14 reference operator*() const noexcept { return *it; }

#if _MYSTL_CXX_VERSION >= 20
    _MYSTL_CONSTEXPR_SINCE_CXX14 pointer operator->() const noexcept
        requires(std::is_pointer_v<Iter> || requires(const Iter __i) { __i.operator->(); })
    {
        if constexpr (std::is_pointer_v<Iter>) {
            return it;
        } else {
            return it.operator->();
        }
    }
#else
    _MYSTL_CONSTEXPR_SINCE_CXX14 pointer operator->() const noexcept { return std::addressof(operator*()); }
#endif // _MYSTL_CXX_VERSION >= 20

    // 自增自减
    _MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter& operator++() noexcept {
        ++it;
        return *this;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter operator++(int) noexcept {
        wrap_iter tmp(*this);
        ++(*this);
        return tmp;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter& operator--() noexcept {
        --it;
        return *this;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter operator--(int) noexcept {
        wrap_iter tmp(*this);
        --(*this);
        return tmp;
    }

    // +, +=, -, -=
    _MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter operator+(difference_type __n) const noexcept {
        wrap_iter tmp(*this);
        tmp += __n;
        return tmp;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter& operator+=(difference_type __n) noexcept {
        it += __n;
        return *this;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter operator-(difference_type __n) const noexcept {
        wrap_iter tmp(*this);
        tmp -= __n;
        return tmp;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter& operator-=(difference_type __n) noexcept {
        it -= __n;
        return *this;
    }

    // [] 运算符
    _MYSTL_CONSTEXPR_SINCE_CXX14 reference operator[](difference_type __n) const noexcept { return it[__n]; }

    // base() 返回被包装前的迭代器
    _MYSTL_CONSTEXPR_SINCE_CXX14 iterator_type base() const noexcept { return it; }

private:
    // 将构造函数设置为私有，避免外部使用
    // 测试时设为public
    _MYSTL_CONSTEXPR_SINCE_CXX14 explicit wrap_iter(iterator_type __x) noexcept : it(__x) {}

    // 在这里将需要用到 wrap_iter 的类设置为 friend
    template <class _Iter>
    friend class wrap_iter;
    template <typename _Tp, class _Allocator>
    friend class vector;
};

// 迭代器之间的比较运算
template <class Iter1>
constexpr bool operator==(const wrap_iter<Iter1>& __x, const wrap_iter<Iter1>& __y) noexcept {
    return __x.base() == __y.base();
}

template <class Iter1, class Iter2>
constexpr bool operator==(const wrap_iter<Iter1>& __x, const wrap_iter<Iter2>& __y) noexcept {
    return __x.base() == __y.base();
}

template <class Iter1>
constexpr bool operator<(const wrap_iter<Iter1>& __x, const wrap_iter<Iter1>& __y) noexcept {
    return __x.base() < __y.base();
}

template <class Iter1, class Iter2>
constexpr bool operator<(const wrap_iter<Iter1>& __x, const wrap_iter<Iter2>& __y) noexcept {
    return __x.base() < __y.base();
}

#if _MYSTL_CXX_VERSION <= 17 // C++ 17 以下需要展开每个比较运算符
template <class Iter1>
constexpr bool operator!=(const wrap_iter<Iter1>& __x, const wrap_iter<Iter1>& __y) noexcept {
    return !(__x.base() == __y.base());
}

template <class Iter1, class Iter2>
constexpr bool operator!=(const wrap_iter<Iter1>& __x, const wrap_iter<Iter2>& __y) noexcept {
    return !(__x.base() == __y.base());
}

template <class Iter1>
constexpr bool operator>(const wrap_iter<Iter1>& __x, const wrap_iter<Iter1>& __y) noexcept {
    return __y < __x;
}

template <class Iter1, class Iter2>
constexpr bool operator>(const wrap_iter<Iter1>& __x, const wrap_iter<Iter2>& __y) noexcept {
    return __y < __x;
}

template <class Iter1>
constexpr bool operator<=(const wrap_iter<Iter1>& __x, const wrap_iter<Iter1>& __y) noexcept {
    return !(__y < __x);
}

template <class Iter1, class Iter2>
constexpr bool operator<=(const wrap_iter<Iter1>& __x, const wrap_iter<Iter2>& __y) noexcept {
    return !(__y < __x);
}

template <class Iter1>
constexpr bool operator>=(const wrap_iter<Iter1>& __x, const wrap_iter<Iter1>& __y) noexcept {
    return !(__x < __y);
}

template <class Iter1, class Iter2>
constexpr bool operator>=(const wrap_iter<Iter1>& __x, const wrap_iter<Iter2>& __y) noexcept {
    return !(__x < __y);
}
#else  // C++20 可以使用三路比较运算符
template <class Iter1, class Iter2>
constexpr std::strong_ordering operator<=>(const wrap_iter<Iter1>& __x, const wrap_iter<Iter2>& __y) noexcept {
    if constexpr (std::three_way_comparable_with<Iter1, Iter2, std::strong_ordering>) {
        return __x.base() <=> __y.base();
    } else {
        if (__x.base() < __y.base()) return std::strong_ordering::less;
        if (__x.base() == __y.base()) return std::strong_ordering::equal;
        return std::strong_ordering::greater;
    }
}
#endif // _MYSTL_CXX_VERSION <= 17

// 迭代器之间的相减
template <class Iter1, class Iter2>
_MYSTL_CONSTEXPR_SINCE_CXX14 auto operator-(const wrap_iter<Iter1>& __x, const wrap_iter<Iter2>& __y) noexcept -> decltype(__x.base() - __y.base()) {
    return __x.base() - __y.base();
}

// 重载 difference_type + wrap_iter
template <class Iter>
_MYSTL_CONSTEXPR_SINCE_CXX14 wrap_iter<Iter> operator+(typename wrap_iter<Iter>::difference_type __n, wrap_iter<Iter> __x) noexcept {
    __x += __n;
    return __x;
}

//===----------------------------------------------------------===//



//===----------------------------------------------------------===//
//===-----------------实现一个 reverse_iterator-----------------===//
#if _MYSTL_CXX_VERSION <= 17 // 至少为 bidirectional_iterator
template <class Iter, std::enable_if_t<is_based_on_bidirectional_iterator<Iter>::value, int> = 0>
#else
template <BasedOnBidirectionalIterator Iter>
#endif // _MYSTL_CXX_VERSION <= 17
class reverse_iterator {
public:
    using iterator_type     = Iter;
    using value_type        = typename iterator_traits<Iter>::value_type;
    using difference_type   = typename iterator_traits<Iter>::difference_type;
    using pointer           = typename iterator_traits<Iter>::pointer;
    using reference         = typename iterator_traits<Iter>::reference;
    using iterator_category = typename iterator_traits<Iter>::iterator_category;
    // std 中, 哪怕是 C++20 也不是所有迭代器都有 iterator_concept 属性，因此这个不使用
    // #if _MYSTL_CXX_VERSION >= 20
    //     using iterator_concept = typename iterator_traits<Iter>::iterator_concept;
    // #endif

protected:
    Iter current;

public:
    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator() : current() {}

    _MYSTL_CONSTEXPR_SINCE_CXX17 explicit reverse_iterator(Iter __x) : current(__x) {}

    // 从类型不同但可转换的另一个Iter进行拷贝构造
    template <class OtherIter, std::enable_if_t<!std::is_same_v<OtherIter, Iter> && std::is_convertible_v<OtherIter const&, Iter>, int> = 0>
    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator(const reverse_iterator<OtherIter>& other) : current(other.base()) {}

    // 从类型不同但可赋值的另一个Iter进行赋值
    template <class OtherIter, std::enable_if_t<!std::is_same_v<OtherIter, Iter> && std::is_convertible_v<OtherIter const&, Iter> &&
                                                    std::is_assignable_v<Iter&, OtherIter const&>,
                                                int> = 0>
    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator& operator=(const reverse_iterator<OtherIter>& other) {
        current = other.base();
        return *this;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX17 Iter base() const { return current; }

    // 反向迭代器返回前一个位置的元素
    _MYSTL_CONSTEXPR_SINCE_CXX17 reference operator*() const {
        Iter tmp = current;
        return *--tmp;
    }

#if _MYSTL_CXX_VERSION >= 20
    _MYSTL_CONSTEXPR_SINCE_CXX17 pointer operator->() const
        requires std::is_pointer_v<Iter> || requires(const Iter __i) { __i.operator->(); }
    {
        Iter tmp = current;
        --tmp;
        if constexpr (std::is_pointer_v<Iter>) {
            return tmp;
        } else {
            return tmp.operator->();
        }
    }
#else
    _MYSTL_CONSTEXPR_SINCE_CXX17 pointer operator->() const { return std::addressof(operator*()); }
#endif // _MYSTL_CXX_VERSION >= 20

    // 定义相反的自增自减运算
    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator& operator++() {
        --current;
        return *this;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator operator++(int) {
        reverse_iterator tmp(*this);
        --current;
        return tmp;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator& operator--() {
        ++current;
        return *this;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator operator--(int) {
        reverse_iterator tmp(*this);
        ++current;
        return tmp;
    }

    // 定义相反的 +, +=, -, -=
    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator operator+(difference_type __n) const { return reverse_iterator(current - __n); }

    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator& operator+=(difference_type __n) {
        current -= __n;
        return *this;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator operator-(difference_type __n) const { return reverse_iterator(current + __n); }

    _MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator& operator-=(difference_type __n) {
        current += __n;
        return *this;
    }

    // [] 运算符
    _MYSTL_CONSTEXPR_SINCE_CXX17 reference operator[](difference_type __n) { return *(*this + __n); }
};

// 迭代器之间的比较运算
template <class Iter1, class Iter2>
_MYSTL_CONSTEXPR_SINCE_CXX17 bool operator==(const reverse_iterator<Iter1>& __x, const reverse_iterator<Iter2>& __y)
#if _MYSTL_CXX_VERSION >= 20
    requires requires {
        { __x.base() == __y.base() } -> std::convertible_to<bool>;
    }
#endif
{
    return __x.base() == __y.base();
}

template <class Iter1, class Iter2>
_MYSTL_CONSTEXPR_SINCE_CXX17 bool operator<(const reverse_iterator<Iter1>& __x, const reverse_iterator<Iter2>& __y)
#if _MYSTL_CXX_VERSION >= 20
    requires requires {
        { __x.base() > __y.base() } -> std::convertible_to<bool>;
    }
#endif
{
    return __x.base() > __y.base(); // reverse_iterator 因此比较也是取反
}

template <class Iter1, class Iter2>
_MYSTL_CONSTEXPR_SINCE_CXX17 bool operator!=(const reverse_iterator<Iter1>& __x, const reverse_iterator<Iter2>& __y)
#if _MYSTL_CXX_VERSION >= 20
    requires requires {
        { __x.base() != __y.base() } -> std::convertible_to<bool>;
    }
#endif
{
    return __x.base() != __y.base();
}

template <class Iter1, class Iter2>
_MYSTL_CONSTEXPR_SINCE_CXX17 bool operator>(const reverse_iterator<Iter1>& __x, const reverse_iterator<Iter2>& __y)
#if _MYSTL_CXX_VERSION >= 20
    requires requires {
        { __x.base() < __y.base() } -> std::convertible_to<bool>;
    }
#endif
{
    return __x.base() < __y.base();
}

template <class Iter1, class Iter2>
_MYSTL_CONSTEXPR_SINCE_CXX17 bool operator>=(const reverse_iterator<Iter1>& __x, const reverse_iterator<Iter2>& __y)
#if _MYSTL_CXX_VERSION >= 20
    requires requires {
        { __x.base() <= __y.base() } -> std::convertible_to<bool>;
    }
#endif
{
    return __x.base() <= __y.base();
}

template <class Iter1, class Iter2>
_MYSTL_CONSTEXPR_SINCE_CXX17 bool operator<=(const reverse_iterator<Iter1>& __x, const reverse_iterator<Iter2>& __y)
#if _MYSTL_CXX_VERSION >= 20
    requires requires {
        { __x.base() >= __y.base() } -> std::convertible_to<bool>;
    }
#endif
{
    return __x.base() >= __y.base();
}

#if _MYSTL_CXX_VERSION >= 20
template <class Iter1, std::three_way_comparable_with<Iter1> Iter2>
constexpr std::compare_three_way_result_t<Iter1, Iter2> operator<=>(const reverse_iterator<Iter1>& __x, const reverse_iterator<Iter2>& __y) {
    return __y.base() <=> __x.base();
}
#endif //_MYSTL_CXX_VERSION >= 20

template <class Iter1, class Iter2>
_MYSTL_CONSTEXPR_SINCE_CXX17 auto operator-(const reverse_iterator<Iter1>& __x,
                                            const reverse_iterator<Iter2>& __y) -> decltype(__y.base() - __x.base()) {
    return __y.base() - __x.base();
}

// 重载 +
template <class Iter>
_MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator<Iter> operator+(typename reverse_iterator<Iter>::difference_type __n,
                                                              const reverse_iterator<Iter>& __x) {
    return reverse_iterator<Iter>(__x.base() - __n);
}

// make_reverse_iterator()
template <class Iter>
_MYSTL_CONSTEXPR_SINCE_CXX17 reverse_iterator<Iter> make_reverse_iterator(Iter __i) {
    return reverse_iterator<Iter>(__i);
}

//===----------------------------------------------------------===//

_MYSTL_END_NAMESPACE_MYSTL

namespace std {
template <typename Iter>
struct iterator_traits<mystl::wrap_iter<Iter>> {
    using iterator_category = typename mystl::wrap_iter<Iter>::iterator_category;
    using value_type        = typename mystl::wrap_iter<Iter>::value_type;
    using difference_type   = typename mystl::wrap_iter<Iter>::difference_type;
    using pointer           = typename mystl::wrap_iter<Iter>::pointer;
    using reference         = typename mystl::wrap_iter<Iter>::reference;
#if _MYSTL_CXX_VERSION >= 20
    using iterator_concept = typename mystl::wrap_iter<Iter>::iterator_concept;
#endif
};
} // namespace std

#endif // _MYSTL_ITERATOR_H
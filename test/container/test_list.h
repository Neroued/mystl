#ifndef _MYSTL_TEST_LIST_H
#define _MYSTL_TEST_LIST_H

#include "test.h"

#include <cassert>
#include <list.h>
#include <list>

_MYSTL_BEGIN_NAMESPACE_MYSTL_TEST

class list_test {
public:
    static void test_all() {
        test_construct();
        test_iterator();
        test_modifier();
        test_operation();
        test_non_member();
    }

    static void test_construct() {
        const int n = 20;
        // 默认构造
        mystl::list<int> l0;
        assert(l0.empty());

        mystl::list<int> l1(n);
        assert(l1.size() == n);

        mystl::list<int> l2(n, 10);
        std::list<int> sl2(n, 10);
        assert(is_same(sl2, l2));

        mystl::list l3(l2.begin(), l2.end());
        std::list sl3(l2.begin(), l2.end());
        assert(is_same(sl3, l3));

        mystl::list l4(l3);
        std::list sl4(sl3);
        assert(is_same(sl4, l4));

        mystl::list l5 = mystl::list<int>(n, 20);
        std::list sl5  = std::list<int>(n, 20);
        assert(is_same(sl5, l5));

        mystl::list l6 = {1, 2, 3, 4, 5};
        std::list sl6  = {1, 2, 3, 4, 5};
        assert(is_same(sl6, l6));

        std::cout << "List construction test passed" << std::endl;
    }

    static void test_iterator() {
        mystl::list lis = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        std::list slis  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        assert(*lis.begin() == *slis.begin());
        assert(*--lis.end() == *--slis.end());
        assert(*++lis.begin() == *++slis.begin());
        assert(*-- --lis.end() == *-- --slis.end());
        assert(*++lis.rbegin() == *++slis.rbegin());
        assert(*--lis.rend() == *--slis.rend());

        std::cout << "List iterator test passed" << std::endl;
    }

    static void test_modifier() {
        mystl::list<int> lis = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        std::list<int> slis  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        // insert
        lis.insert(lis.begin(), 999);
        slis.insert(slis.begin(), 999);
        assert(is_same(slis, lis));

        mystl::list<int> lis2 = {-1, -2, -3};
        std::list<int> slis2  = {-1, -2, -3};

        lis.insert(lis.begin(), lis2.begin(), lis2.end());
        slis.insert(slis.begin(), slis2.begin(), slis2.end());
        assert(is_same(slis, lis));

        lis.insert(lis.begin(), {-10, 2, 3, 999});
        slis.insert(slis.begin(), {-10, 2, 3, 999});
        assert(is_same(slis, lis));

        lis.insert(lis.begin(), 5, 10);
        slis.insert(slis.begin(), 5, 10);
        assert(is_same(slis, lis));

        lis.emplace(lis.begin(), 100);
        slis.emplace(slis.begin(), 100);
        assert(is_same(slis, lis));

        lis.emplace_back(100);
        slis.emplace_back(100);
        assert(is_same(slis, lis));

        lis.emplace_front(100);
        slis.emplace_front(100);
        assert(is_same(slis, lis));

        lis.push_back(187);
        slis.push_back(187);
        assert(is_same(slis, lis));

        lis.push_front(187);
        slis.push_front(187);
        assert(is_same(slis, lis));

        lis.pop_back();
        slis.pop_back();
        assert(is_same(slis, lis));

        lis.pop_front();
        slis.pop_front();
        assert(is_same(slis, lis));

        lis.erase(lis.begin());
        slis.erase(slis.begin());
        assert(is_same(slis, lis));

        mystl::list<int> lis3 = {-1, -2, -3};
        std::list<int> slis3  = {-1, -2, -3};

        lis.swap(lis3);
        slis.swap(slis3);
        assert(is_same(slis, lis));
        assert(is_same(slis3, lis3));

        std::swap(lis, lis3);
        std::swap(slis, slis3);
        assert(is_same(slis, lis));
        assert(is_same(slis3, lis3));

        lis.resize(lis.size() + 5, 88);
        slis.resize(slis.size() + 5, 88);
        assert(is_same(slis, lis));

        lis.resize(lis.size() - 5, 88);
        slis.resize(slis.size() - 5, 88);
        assert(is_same(slis, lis));

        lis.clear();
        slis.clear();
        assert(lis.empty());
        assert(slis.empty());
        assert(is_same(slis, lis));

        std::cout << "List modifier test passed" << std::endl;
    }

    static void test_operation() {
        mystl::list<int> lis1 = {1, 3, 5, 7, 9, 11};
        mystl::list<int> lis2 = {2, 4, 6, 8, 10, 12};
        std::list<int> slis1  = {1, 3, 5, 7, 9, 11};
        std::list<int> slis2  = {2, 4, 6, 8, 10, 12};
        lis1.merge(lis2);
        slis1.merge(slis2);
        assert(is_same(slis1, lis1));
        assert(lis2.empty());
        assert(slis2.empty());

        mystl::list<int> lis3 = {1, 3, 5, 7, 9, 11};
        std::list<int> slis3  = {1, 3, 5, 7, 9, 11};
        lis1.splice(lis1.end(), lis3);
        slis1.splice(slis1.end(), slis3);
        assert(is_same(slis1, lis1));

        mystl::list<int> lis4 = {1, 3, 5, 7, 9, 11};
        std::list<int> slis4  = {1, 3, 5, 7, 9, 11};
        lis1.splice(lis1.begin(), lis4, lis4.begin());
        slis1.splice(slis1.begin(), slis4, slis4.begin());
        assert(is_same(slis1, lis1));

        lis1.splice(lis1.begin(), lis4, lis4.begin(), lis4.end());
        slis1.splice(slis1.begin(), slis4, slis4.begin(), slis4.end());
        assert(is_same(slis1, lis1));

        lis1.unique();
        slis1.unique();
        assert(is_same(slis1, lis1));

        lis1.sort();
        slis1.sort();
        assert(is_same(slis1, lis1));

        lis1.reverse();
        slis1.reverse();
        assert(is_same(slis1, lis1));

        lis1.remove(1);
        slis1.remove(1);
        assert(is_same(slis1, lis1));

        lis1.remove_if([](int a) { return a == 2; });
        slis1.remove_if([](int a) { return a == 2; });
        assert(is_same(slis1, lis1));

        std::cout << "List operation test passed" << std::endl;
    }

    static void test_non_member() {
        // ===========================
        // 1. 空列表的情况
        // ===========================
        mystl::list<int> empty1;
        mystl::list<int> empty2;
        // 空列表应相等
        assert(empty1 == empty2);
        assert(!(empty1 != empty2));
        // 空列表之间不可能有 < 或 >，<= 和 >= 应为 true
        assert(!(empty1 < empty2));
        assert(empty1 <= empty2);
        assert(!(empty1 > empty2));
        assert(empty1 >= empty2);
#if _MYSTL_CXX_VERSION > 17
        auto cmp = empty1 <=> empty2;
        assert(cmp == 0);
#endif

        // ===========================
        // 2. 一个空，一个非空
        // ===========================
        mystl::list<int> oneElem = {1};
        // 空列表视为前缀，所以空 < 非空
        assert(empty1 != oneElem);
        assert(empty1 < oneElem);
        assert(empty1 <= oneElem);
        assert(!(empty1 > oneElem));
        assert(!(empty1 >= oneElem));
#if _MYSTL_CXX_VERSION > 17
        cmp = empty1 <=> oneElem;
        assert(cmp < 0);
        cmp = oneElem <=> empty1;
        assert(cmp > 0);
#endif

        // ===========================
        // 3. 长度相同且所有元素均相等
        // ===========================
        mystl::list<int> a = {1, 2, 3};
        mystl::list<int> b = {1, 2, 3};
        assert(a == b);
        assert(!(a != b));
        assert(!(a < b));
        assert(a <= b);
        assert(!(a > b));
        assert(a >= b);
#if _MYSTL_CXX_VERSION > 17
        cmp = a <=> b;
        assert(cmp == 0);
#endif

        // ===========================
        // 4. 长度相同但存在差异
        // ===========================
        mystl::list<int> c = {1, 3, 5};
        mystl::list<int> d = {1, 4, 2};
        // 比较到第二个元素：3 < 4，因此 c < d
        assert(c != d);
        assert(c < d);
        assert(c <= d);
        assert(!(c > d));
        assert(!(c >= d));
#if _MYSTL_CXX_VERSION > 17
        cmp = c <=> d;
        assert(cmp < 0);
        cmp = d <=> c;
        assert(cmp > 0);
#endif

        // ===========================
        // 5. 一个列表是另一个列表的前缀
        // ===========================
        mystl::list<int> prefix = {1, 2};
        mystl::list<int> longer = {1, 2, 3};
        assert(prefix != longer);
        assert(prefix < longer);
        assert(prefix <= longer);
        assert(longer > prefix);
        assert(longer >= prefix);
#if _MYSTL_CXX_VERSION > 17
        cmp = prefix <=> longer;
        assert(cmp < 0);
        cmp = longer <=> prefix;
        assert(cmp > 0);
#endif

        // ===========================
        // 6. 使用 std::string 类型
        // ===========================
        mystl::list<std::string> s1 = {"apple", "banana"};
        mystl::list<std::string> s2 = {"apple", "cherry"};
        // 对于字符串比较，由于 "banana" < "cherry"，因此 s1 < s2
        assert(s1 != s2);
        assert(s1 < s2);
        assert(s1 <= s2);
        assert(!(s1 > s2));
        assert(!(s1 >= s2));
#if _MYSTL_CXX_VERSION > 17
        auto cmp_str = s1 <=> s2;
        assert(cmp_str < 0);
        cmp_str = s2 <=> s1;
        assert(cmp_str > 0);
#endif

        std::cout << "List non-member test passed" << std::endl;
    }

private:
    template <class _Tp>
    static bool is_same(std::list<_Tp>& l1, mystl::list<_Tp>& l2) {
        if (l1.size() != l2.size()) { return false; }
        auto it1  = l1.begin();
        auto it2  = l2.begin();
        auto end1 = l1.end();
        auto end2 = l2.end();
        while (it1 != end1 && it2 != end2) {
            if (*it1 != *it2) { return false; }
            ++it1;
            ++it2;
        }
        return true;
    }
};

_MYSTL_END_NAMESPACE_MYSTL_TEST

#endif // _MYSTL_TEST_LIST_H
#ifndef _MYSTL_TEST_VECTOR_H
#define _MYSTL_TEST_VECTOR_H

#include "test.h"

#include <cassert>
#include <string>
#include <utility>  
#include <vector.h> 
#include <vector>

_MYSTL_BEGIN_NAMESPACE_MYSTL_TEST

class vector_test {
public:
    static void test_all() {
        test_construct();
        test_iterator();
        test_capacity();
        test_element_access();
        test_modifier();
    }

    static void test_construct() {
        const int n = 20;
        // 默认构造
        mystl::vector<int> v0;
        assert(v0.empty());

        // 构造指定大小，默认值构造
        mystl::vector<int> v1(n);
        assert(v1.size() == n);

        // 构造指定大小及初始值
        mystl::vector<int> v2(n, 10);
        std::vector<int> sv2(n, 10);
        assert(is_same(sv2, v2));

        // 范围构造
        mystl::vector<int> v3(v2.begin(), v2.end());
        std::vector<int> sv3(sv2.begin(), sv2.end());
        assert(is_same(sv3, v3));

        // 拷贝构造
        mystl::vector<int> v4(v3);
        std::vector<int> sv4(sv3);
        assert(is_same(sv4, v4));

        // 移动构造
        mystl::vector<int> v5 = std::move(v4);
        std::vector<int> sv5  = std::move(sv4);
        assert(is_same(sv5, v5));

        // 初始化列表构造
        mystl::vector<int> v6 = {1, 2, 3, 4, 5};
        std::vector<int> sv6  = {1, 2, 3, 4, 5};
        assert(is_same(sv6, v6));

        // 针对 string 类型的测试
        mystl::vector<std::string> vs1 = {"a", "b", "c"};
        std::vector<std::string> svs1  = {"a", "b", "c"};
        assert(is_same(svs1, vs1));

        std::cout << "Vector construction test passed" << std::endl;
    }

    static void test_iterator() {
        mystl::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        std::vector<int> sv  = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        // 测试 begin 与 end
        assert(*v.begin() == *sv.begin());
        assert(*(v.end() - 1) == *(sv.end() - 1));

        // 测试反向迭代器
        assert(*v.rbegin() == *sv.rbegin());
        assert(*(v.rend() - 1) == *(sv.rend() - 1));

        std::cout << "Vector iterator test passed" << std::endl;
    }

    static void test_capacity() {
        mystl::vector<int> v = {1, 2, 3, 4, 5};
        std::vector<int> sv  = {1, 2, 3, 4, 5};

        // 测试 size、empty
        assert(v.size() == sv.size());
        assert(v.empty() == sv.empty());

        // 测试 reserve：reserve 后 size 不变
        v.reserve(100);
        sv.reserve(100);
        assert(v.size() == sv.size());

        // 测试 shrink_to_fit（若实现）
        v.shrink_to_fit();
        sv.shrink_to_fit();
        // 不强求 capacity 相同，但保证 size 保持不变
        assert(v.size() == sv.size());

        std::cout << "Vector capacity test passed" << std::endl;
    }

    static void test_element_access() {
        mystl::vector<int> v = {10, 20, 30, 40, 50};
        std::vector<int> sv  = {10, 20, 30, 40, 50};

        // operator[]
        for (size_t i = 0; i < v.size(); ++i) { assert(v[i] == sv[i]); }
        // at()
        for (size_t i = 0; i < v.size(); ++i) { assert(v.at(i) == sv.at(i)); }
        // front() 与 back()
        assert(v.front() == sv.front());
        assert(v.back() == sv.back());
        // data()
        auto data_v  = v.data();
        auto data_sv = sv.data();
        for (size_t i = 0; i < v.size(); ++i) { assert(data_v[i] == data_sv[i]); }

        std::cout << "Vector element access test passed" << std::endl;
    }

    static void test_modifier() {
        mystl::vector<int> v = {1, 2, 3, 4, 5};
        std::vector<int> sv  = {1, 2, 3, 4, 5};

        // assign(count, value)
        v.assign(10, 99);
        sv.assign(10, 99);
        assert(is_same(sv, v));

        // assign 范围赋值
        std::vector<int> tmp = {7, 8, 9};
        v.assign(tmp.begin(), tmp.end());
        sv.assign(tmp.begin(), tmp.end());
        assert(is_same(sv, v));

        // push_back
        v.push_back(100);
        sv.push_back(100);
        assert(is_same(sv, v));

        // pop_back
        v.pop_back();
        sv.pop_back();
        assert(is_same(sv, v));

        // insert 单个元素
        v.insert(v.begin() + 1, 200);
        sv.insert(sv.begin() + 1, 200);
        assert(is_same(sv, v));

        // insert 多个元素
        v.insert(v.begin() + 2, 3, 300);
        sv.insert(sv.begin() + 2, 3, 300);
        assert(is_same(sv, v));

        // insert 初始化列表
        v.insert(v.begin(), {400, 500});
        sv.insert(sv.begin(), {400, 500});
        assert(is_same(sv, v));

        // emplace
        v.emplace(v.begin(), 111);
        sv.emplace(sv.begin(), 111);
        assert(is_same(sv, v));

        // emplace_back
        v.emplace_back(222);
        sv.emplace_back(222);
        for (auto x : v) { std::cout << x << " "; }
        std::cout << std::endl;
        for (auto x : sv) { std::cout << x << " "; }
        std::cout << std::endl;
        assert(is_same(sv, v));

        // erase 单个元素
        v.erase(v.begin());
        sv.erase(sv.begin());
        assert(is_same(sv, v));

        // erase 范围
        if (v.size() > 3) { // 保证大小足够
            v.erase(v.begin(), v.begin() + 2);
            sv.erase(sv.begin(), sv.begin() + 2);
            assert(is_same(sv, v));
        }

        // clear
        v.clear();
        sv.clear();
        assert(v.empty() && sv.empty());

        // swap 成员函数
        mystl::vector<int> v1 = {1, 2, 3};
        mystl::vector<int> v2 = {4, 5, 6};
        std::vector<int> sv1  = {1, 2, 3};
        std::vector<int> sv2  = {4, 5, 6};
        v1.swap(v2);
        sv1.swap(sv2);
        assert(is_same(sv1, v1));
        assert(is_same(sv2, v2));

        // resize
        mystl::vector<int> v3 = {1, 2, 3, 4};
        std::vector<int> sv3  = {1, 2, 3, 4};
        v3.resize(6, 77);
        sv3.resize(6, 77);
        assert(is_same(sv3, v3));
        v3.resize(3);
        sv3.resize(3);
        assert(is_same(sv3, v3));

        std::cout << "Vector modifier test passed" << std::endl;
    }


private:
    template <class T>
    static bool is_same(const std::vector<T>& sv, const mystl::vector<T>& v) {
        if (sv.size() != v.size()) return false;
        auto it1 = sv.begin();
        auto it2 = v.begin();
        while (it1 != sv.end() && it2 != v.end()) {
            if (*it1 != *it2) return false;
            ++it1;
            ++it2;
        }
        return true;
    }
};

_MYSTL_END_NAMESPACE_MYSTL_TEST

#endif // _MYSTL_TEST_VECTOR_H6

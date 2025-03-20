#include "list.h"

#include "allocator.h"
#include "vector.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator.h>
#include <list>



constexpr size_t NUM_ELEMENTS = 20;

// 非平凡类
struct NonTrivialData {
    NonTrivialData() {
        c    = count;
        flag = true;
        data = new int[10];
        count += 1;
    }

    ~NonTrivialData() { delete[] data; }

    NonTrivialData(const NonTrivialData& other) {
        c    = other.c;
        flag = other.flag;
        data = new int[10];
        // for (int i = 0; i < 10; ++i) { data[i] = other.data[i]; }
        std::copy(other.data, other.data + 10, data);
    }

    NonTrivialData& operator=(const NonTrivialData& other) {
        if (this != &other) {
            delete[] data;
            data = new int[10];
            // for (int i = 0; i < 10; ++i) { data[i] = other.data[i]; }
            std::copy(other.data, other.data + 10, data);
            c    = other.c;
            flag = other.flag;
        }
        return *this;
    }

    // 移动构造
    // NonTrivialData(NonTrivialData&& other) noexcept {
    //     data       = other.data;
    //     other.data = nullptr;
    // }

    // 移动赋值
    // NonTrivialData& operator=(NonTrivialData&& other) noexcept {
    //     if (this != &other) {
    //         delete[] data;
    //         data       = other.data;
    //         other.data = nullptr;
    //     }
    //     return *this;
    // }

    int c     = 0;
    bool flag = false;
    int* data = nullptr;
    static int count;
};

int NonTrivialData::count = 0;

// 平凡类
struct TrivialData {
    int c;
    bool flag;
    int data[10];

    TrivialData()                              = default;
    TrivialData(const TrivialData&)            = default;
    TrivialData& operator=(const TrivialData&) = default;
    ~TrivialData()                             = default;
};

template <typename List>
void benchmark(const char* listName) {
    auto start = std::chrono::high_resolution_clock::now();
    List lis({10, 100, 4242, 99});
    for (int i = 0; i < NUM_ELEMENTS; ++i) { lis.emplace_back(); }
    List lis2(lis);
    List lis3 = lis2;

    std::cout << "size: " << lis3.size() << std::endl;

    auto targetIt = std::next(lis3.begin(), 5);
    lis3.insert(targetIt, 5, 5);
    std::cout << "size: " << lis3.size() << std::endl;

    std::vector vec = {1, 2, 3, 4, 5, 6, 7};
    targetIt        = std::next(lis3.begin(), 5);
    lis3.insert(targetIt, vec.begin(), vec.end());
    std::cout << "size: " << lis3.size() << std::endl;

    targetIt = std::next(lis3.begin(), 5);
    lis3.insert(targetIt, {999, 888, 777, 666, 555});
    std::cout << "size: " << lis3.size() << std::endl;

    *(--lis3.end()) = 999;
    lis3.pop_back();

    lis3.pop_front();
    std::cout << "size: " << lis3.size() << std::endl;

    lis3.resize(10);
    std::cout << "size: " << lis3.size() << std::endl;

    lis3.resize(15, 100);
    std::cout << "size: " << lis3.size() << std::endl;

    List list_splice1 = {1, 2, 3, 4, 5};
    List list_splice2 = {6, 7, 8, 9, 10};

    list_splice1.splice(list_splice1.begin(), list_splice2, list_splice2.begin());
    for (auto x : list_splice1) { std::cout << x << ", "; }
    std::cout << std::endl;
    for (auto x : list_splice2) { std::cout << x << ", "; }
    std::cout << std::endl;

    list_splice1.splice(list_splice1.begin(), list_splice2);
    for (auto x : list_splice1) { std::cout << x << ", "; }
    std::cout << std::endl;
    for (auto x : list_splice2) { std::cout << x << ", "; }
    std::cout << std::endl;

    List list_splice3 = {16, 17, 18, 19, 20};
    list_splice1.splice(list_splice1.begin(), std::move(list_splice3), list_splice3.begin(), list_splice3.end());
    for (auto x : list_splice1) { std::cout << x << ", "; }
    std::cout << std::endl;
    for (auto x : list_splice3) { std::cout << x << ", "; }
    std::cout << std::endl;

    for (auto x : lis3) { std::cout << x << ", "; }
    std::cout << std::endl;

    auto n = lis3.unique([](int a, int b) { return std::abs(b - a) < 100; });
    for (auto x : lis3) { std::cout << x << ", "; }
    std::cout << std::endl;
    std::cout << "n: " << n << std::endl;

    lis3.sort();
    for (auto x : lis3) { std::cout << x << ", "; }
    std::cout << std::endl;

    lis3.sort([](int a, int b) { return a > b; });
    for (auto x : lis3) { std::cout << x << ", "; }
    std::cout << std::endl;

    List list_merge1 = {1, 3, 5, 7, 9};
    List list_merge2 = {2, 4, 4, 4, 4, 5, 6, 8, 10};
    list_merge1.merge(list_merge2, [](int a, int b) { return a < b; });
    for (auto x : list_merge1) { std::cout << x << ", "; }
    std::cout << std::endl;

    List list_remove = {1, 2, 3, 3, 4, 2, 2, 1, 1, 2, 1, 1, 1};
    for (auto x : list_remove) { std::cout << x << ", "; }
    std::cout << std::endl;
    list_remove.remove(1);
    for (auto x : list_remove) { std::cout << x << ", "; }
    std::cout << std::endl;
    list_remove.remove(2);
    for (auto x : list_remove) { std::cout << x << ", "; }
    std::cout << std::endl;

    List list_remove_if = {1, 2, 3, 3, 4, 2, 2, 1, 1, 2, 1, 1, 1};
    for (auto x : list_remove_if) { std::cout << x << ", "; }
    std::cout << std::endl;
    list_remove_if.remove_if([](int& a) { return a == 1; });
    for (auto x : list_remove_if) { std::cout << x << ", "; }
    std::cout << std::endl;
    list_remove_if.remove_if([](int& a) { return a == 2; });
    for (auto x : list_remove_if) { std::cout << x << ", "; }
    std::cout << std::endl;


    auto stop    = std::chrono::high_resolution_clock::now();
    auto elapsed = stop - start;
    std::cout << listName << " elapsed time: " << elapsed.count() << " ns\n";

    std::cout << "-------------------------\n";
}

template <typename _Tp>
using std_allocator = std::allocator<_Tp>;
template <typename _Tp>
using mystl_allocator = mystl::allocator<_Tp>;

int main() {
    // 使用标准 list 进行测试 int
    benchmark<std::list<int, std_allocator<int>>>("std::list<int>");

    // 使用自定义 list 进行测试 int
    benchmark<mystl::list<int, mystl_allocator<int>>>("mystl::list<int>");

    // // 使用标准 list 进行测试 TrivialData
    // benchmark<std::list<TrivialData, std_allocator<TrivialData>>>("std::list<TrivialData>");

    // // 使用自定义 list 进行测试 TrivialData
    // benchmark<mystl::list<TrivialData, mystl_allocator<TrivialData>>>("mystl::list<TrivialData>");

    // std::cout << "benchmark TrivialData ok." << std::endl;

    // // 使用标准 list 进行测试 NonTrivialData
    // benchmark<std::list<NonTrivialData, std_allocator<NonTrivialData>>>("std::list<NonTrivialData>");

    // // 使用自定义 list 进行测试 NonTrivialData
    // benchmark<mystl::list<NonTrivialData, mystl_allocator<NonTrivialData>>>("mystl::list<NonTrivialData>");

    // // 使用标准 list 进行测试 std::string
    // benchmark<std::list<std::string, std_allocator<std::string>>>("std::list<std::string>");

    // // 使用自定义 list 进行测试 std::string
    // benchmark<mystl::list<std::string, mystl_allocator<std::string>>>("mystl::list<std::string>");

    // std::cout << "benchmark NonTrivialData ok." << std::endl;

    // mystl::list<int, mystl::allocator<int>> vec4({1, 2, 3, 4, 5});
    // for (auto x : vec4) { std::cout << x << ", "; }
    // std::cout << std::endl;

    // mystl::list<int> vec = mystl::list<int>(10);
    // mystl::list vec2(vec.begin(), vec.end());

    // std::list stdvec(vec.begin(), vec.end());

    mystl::list<int> lis1 = mystl::list<int>({1, 2, 3, 3, 5});

    return 0;
}

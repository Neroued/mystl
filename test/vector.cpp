#include "vector.h"

#include "allocator.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator.h>
#include <vector>



constexpr size_t NUM_ELEMENTS = 1000;

// 非平凡类
struct NonTrivialData {
    NonTrivialData() {
        c    = count;
        flag = true;
        data = new int[10];
        count += 1;
    }

    ~NonTrivialData() {
        delete[] data;
    }

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

template <typename Vector>
void benchmark(const char* vectName) {
    auto start = std::chrono::high_resolution_clock::now();
    Vector vec;
    vec.reserve(NUM_ELEMENTS);
    for (int i = 0; i < NUM_ELEMENTS; ++i) { vec.emplace_back(); }

    // for (int i = 0; i < 15; ++i) { std::cout << vec[i] << ", "; }
    // std::cout << std::endl;

    // Vector vect = Vector(10, 100);

    
    // typename Vector::reference x = vec[NUM_ELEMENTS / 2 - 5];
    auto& x = vec[10];
    // auto it = vec.insert(vec.begin() + 1, vect.begin() + 10, vect.end());
    // auto it = vec.emplace(vec.begin() + 5, 1000);
    // auto it = vec.erase(vec.begin() + 5, vec.begin() + 100);
    // vect.assign({999, 123123, 12313213, 6, 5, 1235});
    // vec.assign(vect.begin(), vect.end());
    vec.shrink_to_fit();



    // for (int i = 0; i < std::min(static_cast<decltype(vect.size())>(15), vect.size()); ++i) { std::cout << vect[i] << ", "; }
    // std::cout << std::endl;
    // std::cout << "*it = " << *it << std::endl;
    // std::cout << "*++it = " << *++it << std::endl;

    // vec.shrink_to_fit();



    // Vector vec3(vec);
    auto stop    = std::chrono::high_resolution_clock::now();
    auto elapsed = stop - start;
    std::cout << vectName << " elapsed time: " << elapsed.count() << " ns\n";

    std::cout << "-------------------------\n";
}

template <typename _Tp>
using std_allocator = mystl::allocator<_Tp>;
template <typename _Tp>
using mystl_allocator = mystl::allocator<_Tp>;

int main() {
    // 使用标准 vector 进行测试 int
    benchmark<std::vector<int, std_allocator<int>>>("std::vector<int>");

    // 使用自定义 vector 进行测试 int
    benchmark<mystl::vector<int, mystl_allocator<int>>>("mystl::vector<int>");

    // 使用标准 vector 进行测试 TrivialData
    benchmark<std::vector<TrivialData, std_allocator<TrivialData>>>("std::vector<TrivialData>");

    // 使用自定义 vector 进行测试 TrivialData
    benchmark<mystl::vector<TrivialData, mystl_allocator<TrivialData>>>("mystl::vector<TrivialData>");

    std::cout << "benchmark TrivialData ok." << std::endl;

    // 使用标准 vector 进行测试 NonTrivialData
    benchmark<std::vector<NonTrivialData, std_allocator<NonTrivialData>>>("std::vector<NonTrivialData>");

    // 使用自定义 vector 进行测试 NonTrivialData
    benchmark<mystl::vector<NonTrivialData, mystl_allocator<NonTrivialData>>>("mystl::vector<NonTrivialData>");

    // 使用标准 vector 进行测试 std::string
    benchmark<std::vector<std::string, std_allocator<std::string>>>("std::vector<std::string>");

    // 使用自定义 vector 进行测试 std::string
    benchmark<mystl::vector<std::string, mystl_allocator<std::string>>>("mystl::vector<std::string>");

    std::cout << "benchmark NonTrivialData ok." << std::endl;

    mystl::vector<int, mystl::allocator<int>> vec4({1, 2, 3, 4, 5});
    for (auto x : vec4) { std::cout << x << ", "; }
    std::cout << std::endl;

    mystl::vector<int> vec = mystl::vector<int>(10);
    mystl::vector vec2(vec.begin(), vec.end());

    std::vector stdvec(vec.begin(), vec.end());

    return 0;
}

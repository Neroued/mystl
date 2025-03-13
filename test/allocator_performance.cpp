#include "allocator.h"

#include <chrono>
#include <iostream>
#include <vector>


constexpr size_t NUM_ELEMENTS = 1000000;

template <typename Alloc>
void benchmark(const char* allocName) {
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int, Alloc> vec;
    // vec.reserve(NUM_ELEMENTS); // 避免因动态扩容影响性能
    for (size_t i = 0; i < NUM_ELEMENTS; ++i) { vec.push_back(static_cast<int>(i)); }

    auto end     = std::chrono::high_resolution_clock::now();
    auto elapsed = end - start;
    std::cout << allocName << " elapsed time: " << elapsed.count() << " ns\n";
}

int main() {
    // 使用自定义 allocator 进行测试
    benchmark<mystl::allocator<int>>("mystl::allocator");

    // 使用标准 allocator 进行测试
    benchmark<std::allocator<int>>("  std::allocator");

    return 0;
}

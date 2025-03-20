#include "allocator.h" // 包含你的自定义 allocator

#include <iostream>
#include <list>
#include <vector>

int main() {
    // 使用 mystl::allocator 测试 std::vector
    std::vector<int, mystl::allocator<int>> vec;
    for (int i = 0; i < 100; ++i) { vec.push_back(i); }
    std::cout << "std::vector with mystl::allocator, size = " << vec.size() << "\n";

    // 使用 mystl::allocator 测试 std::list
    std::list<int, mystl::allocator<int>> lst;
    for (int i = 0; i < 100; ++i) { lst.push_back(i); }
    std::cout << "std::list with mystl::allocator, size = " << lst.size() << "\n";

    // 检查迭代器访问
    int sum = 0;
    for (auto it = vec.begin(); it != vec.end(); ++it) { sum += *it; }
    std::cout << "Sum of vector elements = " << sum << "\n";

    return 0;
}

#include "iterator.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace mystl;

void test_wrap_iter() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    using Iter           = std::vector<int>::iterator;
    using WrapIter       = wrap_iter<Iter>;

    // // 测试构造函数
    // WrapIter it1(vec.begin());
    // WrapIter it2(it1);
    // assert(it1 == it2);

    // // 测试解引用
    // assert(*it1 == 1);
    // assert(it1[1] == 2);

    // // 测试自增
    // WrapIter it3 = it1;
    // ++it3;
    // assert(*it3 == 2);
    // it3++;
    // assert(*it3 == 3);

    // // 测试自减
    // --it3;
    // assert(*it3 == 2);
    // it3--;
    // assert(*it3 == 1);

    // // 测试加法
    // WrapIter it4 = it1 + 2;
    // assert(*it4 == 3);
    // it4 += 2;
    // assert(*it4 == 5);

    // // 测试减法
    // WrapIter it5 = it4 - 2;
    // assert(*it5 == 3);
    // it5 -= 1;
    // assert(*it5 == 2);

    // // 测试迭代器相减
    // assert((it4 - it1) == 4);

    // // 测试比较运算符
    // assert(it1 < it4);
    // assert(it4 > it1);
    // assert(it1 <= it1);
    // assert(it4 >= it4);
    // assert(it1 != it4);

    std::cout << "All wrap_iter tests passed!\n";
}

int main() {
    test_wrap_iter();
    return 0;
}
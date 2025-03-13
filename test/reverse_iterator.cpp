#include "iterator.h"

#include <cassert>
#include <iostream>
#include <vector>

using namespace mystl;

void test_reverse_iterator() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    using Iter        = std::vector<int>::iterator;
    using ReverseIter = reverse_iterator<Iter>;

    // 测试构造函数
    Iter it = vec.end();
    ReverseIter rit1(it);
    ReverseIter rit2 = rit1;
    assert(rit1 == rit2);

    // 测试解引用
    assert(*rit1 == 5);
    assert(rit1[1] == 4);

    // 测试自增
    ReverseIter rit3 = rit1;
    ++rit3;
    assert(*rit3 == 4);
    rit3++;
    assert(*rit3 == 3);

    // 测试自减
    --rit3;
    assert(*rit3 == 4);
    rit3--;
    assert(*rit3 == 5);

    // 测试加法
    ReverseIter rit4 = rit1 + 2;
    assert(*rit4 == 3);
    rit4 += 2;
    assert(*rit4 == 1);

    // 测试减法
    ReverseIter rit5 = rit4 - 2;
    assert(*rit5 == 3);
    rit5 -= 1;
    assert(*rit5 == 4);

    // 测试迭代器相减
    assert((rit4 - rit1) == 4);

    // 测试比较运算符
    assert(rit1 < rit4);
    assert(rit4 > rit1);
    assert(rit1 <= rit1);
    assert(rit1 >= rit1);
    assert(rit1 != rit4);

    std::cout << "All reverse_iterator tests passed!\n";
}

int main() {
    test_reverse_iterator();
    return 0;
}
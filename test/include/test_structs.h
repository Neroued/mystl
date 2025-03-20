#ifndef _MYSTL_TEST_STRUCTS_H
#define _MYSTL_TEST_STRUCTS_H

#include <algorithm>
#include <test.h>

_MYSTL_BEGIN_NAMESPACE_MYSTL_TEST

struct WrapInt {
    int value;
    static unsigned int count;

    WrapInt() : value(count) { count += 1; }
};

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
        std::copy(other.data, other.data + 10, data);
    }

    NonTrivialData& operator=(const NonTrivialData& other) {
        if (this != &other) {
            delete[] data;
            data = new int[10];
            std::copy(other.data, other.data + 10, data);
            c    = other.c;
            flag = other.flag;
        }
        return *this;
    }

    // 移动构造
    NonTrivialData(NonTrivialData&& other) noexcept {
        data       = other.data;
        other.data = nullptr;
    }

    // 移动赋值
    NonTrivialData& operator=(NonTrivialData&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data       = other.data;
            other.data = nullptr;
        }
        return *this;
    }

    int c     = 0;
    bool flag = false;
    int* data = nullptr;
    static unsigned int count;
};

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

_MYSTL_END_NAMESPACE_MYSTL_TEST

#endif // _MYSTL_TEST_STRUCTS_H
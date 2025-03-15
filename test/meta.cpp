#include <iostream>

static unsigned long long count = 0;

template <unsigned int N>
struct Fibonacci {
    static constexpr double value = Fibonacci<N - 1>::value + Fibonacci<N - 2>::value;
};

template <>
struct Fibonacci<0> {
    static constexpr double value = 1;
};

template <>
struct Fibonacci<1> {
    static constexpr double value = 1;
};

int main() {
    std::cout << "Fibonacci(48) = " << Fibonacci<48>::value << std::endl;
    std::cout << "Fibonacci(49) = " << Fibonacci<49>::value << std::endl;
    std::cout << "Fibonacci(50) = " << Fibonacci<100>::value << std::endl;
    return 0;
}
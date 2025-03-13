# Vector

1. libcxx vector.h中，`__construct_at_end`注释有问题，应该为：

```c++
// line 893 - 897
//  Default constructs __n objects starting at __end_
//  throws if construction throws
//  Precondition:  __n > 0
//  Precondition:  size() + __n <= capacity()
//  Postcondition:  size() == size() + __n      // 这里最好加一个old
//  Postcondition:  size() == old size() + __n  // 像这样

// line 907 - 912
// Copy constructs __n objects at __end from __x
// throws if construction throws
// Precondition: __n > 0
// Precondition: size() + __n <= capacity()
// Postcondition: size() == old size() + __n
// Postcondition:  [i] == __x for all i in [size() - __n, __n)  // 这一行有点问题
// Postcondition: [i] == __x for all i in [size()- __n, size()) // 应该是这样
```

- emplace
- erase
- swap
- assign
- CTAD
- resize
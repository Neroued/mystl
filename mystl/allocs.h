//===-------------------------------------===//
//
// allocs.h
// 暂时简单封装 new 与 delete
// 以字节数为单位分配
// TODO: 提供更多的内存分配策略
//
//===-------------------------------------===//

#ifndef _MYSTL_ALLOCS_H
#define _MYSTL_ALLOCS_H

#include <config.h>

_MYSTL_BEGIN_NAMESPACE_MYSTL

// 暂时不用constexpr
class alloc {
public:
    [[nodiscard]] static void* allocate(size_t size) { return ::operator new(size); }

    static void deallocate(void* ptr, size_t bytes) noexcept { return ::operator delete(ptr); }
};

_MYSTL_END_NAMESPACE_MYSTL

#endif //_MYSTL_ALLOC_H
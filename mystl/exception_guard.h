#ifndef _MYSTL_EXCEPTION_GUARD_H
#define _MYSTL_EXCEPTION_GUARD_H

#include <config.h>
#include <type_traits>

_MYSTL_BEGIN_NAMESPACE_MYSTL

// __exception_guard 是一个提供异常安全 (strong exception guarantee) 的辅助类，用于管理代码中的回滚逻辑
// 避免手写 try-catch 逻辑， 自动管理回滚
// 创建时接收一个回滚逻辑
// 析构时若异常发生，即 __complete() 没有被调用，则执行回滚
// 若操作成功，需要用户调用 __complete()。
// 在 -fno-exceptions 编译条件下为空操作

template <class _Rollback>
struct __exception_guard_exceptions {
    __exception_guard_exceptions() = delete;

    _MYSTL_CONSTEXPR_SINCE_CXX20 explicit __exception_guard_exceptions(_Rollback __rollback)
        : __rollback_(std::move(__rollback)), __completed_(false) {}

    _MYSTL_CONSTEXPR_SINCE_CXX20
    __exception_guard_exceptions(__exception_guard_exceptions&& other) noexcept(std::is_nothrow_move_constructible_v<_Rollback>)
        : __rollback_(std::move(other.__rollback_)), __completed_(other.__completed_) {
        other.__completed_ = true;
    }

    // 禁用拷贝构造，拷贝复制和移动赋值
    __exception_guard_exceptions(__exception_guard_exceptions const&)            = delete;
    __exception_guard_exceptions& operator=(__exception_guard_exceptions const&) = delete;
    __exception_guard_exceptions& operator=(__exception_guard_exceptions&&)      = delete;

    _MYSTL_CONSTEXPR_SINCE_CXX20 void __complete() noexcept {
        __completed_ = true;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 ~__exception_guard_exceptions() {
        if (!__completed_) __rollback_();
    }

private:
    _Rollback __rollback_;
    bool __completed_;
};

// 在 -fno-exceptions 编译条件下的空操作
template <class _Rollback>
struct __exception_guard_noexceptions {
    __exception_guard_noexceptions() = delete;

    _MYSTL_CONSTEXPR_SINCE_CXX20 explicit __exception_guard_noexceptions(_Rollback __rollback) : __completed_(false) {}

    _MYSTL_CONSTEXPR_SINCE_CXX20
    __exception_guard_noexceptions(__exception_guard_noexceptions&& other) noexcept(std::is_nothrow_move_constructible_v<_Rollback>)
        : __completed_(other.__completed_) {
        other.__completed_ = true;
    }

    // 禁用拷贝构造，拷贝复制和移动赋值
    __exception_guard_noexceptions(__exception_guard_noexceptions const&)            = delete;
    __exception_guard_noexceptions& operator=(__exception_guard_noexceptions const&) = delete;
    __exception_guard_noexceptions& operator=(__exception_guard_noexceptions&&)      = delete;

    _MYSTL_CONSTEXPR_SINCE_CXX20 void __complete() noexcept {
        __completed_ = true;
    }

    _MYSTL_CONSTEXPR_SINCE_CXX20 ~__exception_guard_noexceptions() {}

private:
    bool __completed_;
};

// 根据编译条件选择
#if _MYSTL_HAS_EXCEPTIONS
template <class _Rollback>
using __exception_guard = __exception_guard_exceptions<_Rollback>;
#else
template <class _Rollback>
using __exception_guard = __exception_guard_noexceptions<_Rollback>;
#endif // _MYSTL_HAS_EXCEPTION

template <class _Rollback>
constexpr __exception_guard<_Rollback> __make_exception_guard(_Rollback __rollback) {
    return __exception_guard<_Rollback>(std::move(__rollback));
}

_MYSTL_END_NAMESPACE_MYSTL

#endif // _MYSTL_EXCEPTION_GUARD_H
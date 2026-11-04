#ifndef TTAS_SPINLOCK_H
#define TTAS_SPINLOCK_H

#include <pthread.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

// 使用 int 作为锁变量（0=未加锁，1=已加锁）
typedef int ttas_lock_t;

// 原子 Test-and-Set 操作（跨平台：GCC/Clang 使用 __sync 内建，MSVC 使用互锁 intrinsic）
static inline int test_and_set(int* lock) {
#if defined(_MSC_VER)
    return (int)_InterlockedExchange((long*)lock, 1);
#else
    return __sync_lock_test_and_set(lock, 1);
#endif
}

// 加锁函数（TTAS 实现）
static inline void ttas_lock(ttas_lock_t* lock) {
    while (1) {
        // 非原子测试锁状态
        if (*lock == 0) {
            // 尝试原子获取锁
            if (test_and_set(lock) == 0) {
                return; // 成功获取锁
            }
        }
    }
}

// 解锁函数
static inline void ttas_unlock(ttas_lock_t* lock) {
    *lock = 0;
}

#endif // TTAS_SPINLOCK_H

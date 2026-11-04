#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdio.h>
#include <pthread.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

// 自旋锁类型定义
typedef int spinlock_t;

// 原子 Test-and-Set 操作（跨平台：GCC/Clang 使用 __sync 内建，MSVC 使用互锁 intrinsic）
static inline int test_and_set(int* lock) {
#if defined(_MSC_VER)
    return (int)_InterlockedExchange((long*)lock, 1);
#else
    return __sync_lock_test_and_set(lock, 1);
#endif
}

// 加锁
static inline void spin_lock(spinlock_t* lock) {
    while (test_and_set(lock)) {
        // 自旋等待，可加入 pause() 或 usleep(0) 优化 CPU 占用率
    }
}

// 解锁
static inline void spin_unlock(spinlock_t* lock) {
#if defined(_MSC_VER)
    *lock = 0;
#else
    __sync_lock_release(lock);
#endif
}

#endif // SPINLOCK_H

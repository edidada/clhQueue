#ifndef TTAS_SPINLOCK_H
#define TTAS_SPINLOCK_H

#include <pthread.h>

// 使用 int 作为锁变量（0=未加锁，1=已加锁）
typedef int ttas_lock_t;

// 原子 Test-and-Set 操作（x86 平台）
static inline int test_and_set(int* lock) {
    int old;
    __asm__ volatile(
        "xchg %0, %1"
        : "=r"(old), "+m"(*lock)
        : "0"(1)
        : "memory");
    return old;
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
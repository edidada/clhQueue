#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdio.h>
#include <pthread.h>

// 自旋锁类型定义
typedef int spinlock_t;

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

// 加锁
static inline void spin_lock(spinlock_t* lock) {
    while (test_and_set(lock)) {
        // 自旋等待，可加入 pause() 或 usleep(0) 优化 CPU 占用率
    }
}

// 解锁
static inline void spin_unlock(spinlock_t* lock) {
    *lock = 0;
}

#endif // SPINLOCK_H
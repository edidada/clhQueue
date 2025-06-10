#ifndef PETERSON_LOCK_H
#define PETERSON_LOCK_H

#include <stdbool.h>

typedef struct {
    volatile bool flag[2];   // 表示线程 0 和 1 是否想进入临界区
    volatile int turn;       // 当前轮到谁
} peterson_lock_t;

// 初始化锁
static inline void peterson_lock_init(peterson_lock_t* lock) {
    lock->flag[0] = false;
    lock->flag[1] = false;
    lock->turn = 0;
}

// 加锁函数
static inline void peterson_lock(peterson_lock_t* lock, int thread_id) {
    int other = 1 - thread_id;
    lock->flag[thread_id] = true;
    lock->turn = other;
    while (lock->flag[other] && lock->turn == other) {
        // 自旋等待
    }
}

// 解锁函数
static inline void peterson_unlock(peterson_lock_t* lock, int thread_id) {
    lock->flag[thread_id] = false;
}

#endif // PETERSON_LOCK_H
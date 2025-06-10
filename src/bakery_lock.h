#ifndef BAKERY_LOCK_H
#define BAKERY_LOCK_H

#include <stdbool.h>

#define MAX_THREADS 2  // 可根据需要扩展

typedef struct {
    volatile int choosing[MAX_THREADS];  // 是否正在取号
    volatile int number[MAX_THREADS];    // 当前线程的号码
} bakery_lock_t;

// 初始化锁
static inline void bakery_lock_init(bakery_lock_t* lock) {
    for (int i = 0; i < MAX_THREADS; ++i) {
        lock->choosing[i] = 0;
        lock->number[i] = 0;
    }
}

// 加锁函数
static inline void bakery_lock(bakery_lock_t* lock, int thread_id) {
    lock->choosing[thread_id] = 1;

    // 获取最大号码并加一
    int max_num = 0;
    for (int i = 0; i < MAX_THREADS; ++i) {
        if (lock->number[i] > max_num)
            max_num = lock->number[i];
    }

    lock->number[thread_id] = max_num + 1;
    lock->choosing[thread_id] = 0;

    // 等待其他线程完成取号，并比较优先级
    for (int other = 0; other < MAX_THREADS; ++other) {
        while (lock->choosing[other]) {
            // 等待其他线程完成取号
        }

        // 比较编号大小，若相等则线程号较小者优先
        while (lock->number[other] != 0 &&
               (lock->number[other] < lock->number[thread_id] ||
                (lock->number[other] == lock->number[thread_id] && other < thread_id))) {
            // 自旋等待
                }
    }
}

// 解锁函数
static inline void bakery_unlock(bakery_lock_t* lock, int thread_id) {
    lock->number[thread_id] = 0;
}

#endif // BAKERY_LOCK_H
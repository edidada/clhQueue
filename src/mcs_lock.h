#ifndef MCS_LOCK_H
#define MCS_LOCK_H
#include <stddef.h>
#include <stdatomic.h>
#include <stdbool.h>

typedef struct mcs_node {
    struct mcs_node* volatile next;
    atomic_bool waiting; // 是否正在等待
} mcs_node_t;

typedef struct {
    mcs_node_t* volatile tail;
} mcs_lock_t;

// 初始化锁
static inline void mcs_lock_init(mcs_lock_t* lock) {
    lock->tail = NULL;
}

// 加锁操作
static inline void mcs_lock(mcs_lock_t* lock, mcs_node_t* node) {
    node->next = NULL;
    node->waiting = true;

    // 将当前节点插入到队列尾部，并获取旧的尾节点
    mcs_node_t* prev = atomic_exchange_explicit(&lock->tail, node, memory_order_acq_rel);

    if (prev != NULL) {
        prev->next = node;
        // 自旋等待被唤醒
        while (node->waiting) {
            // 可以加入 pause() 或 usleep(0) 优化
        }
    }

    // 成功获得锁
}

// 解锁操作
static inline void mcs_unlock(mcs_lock_t* lock, mcs_node_t* node) {
    if (node->next == NULL) {
        // 如果没有后续节点，则尝试将 tail 置为当前节点
        mcs_node_t* expected = node;
        if (!atomic_compare_exchange_strong_explicit(
                &lock->tail, &expected, NULL,
                memory_order_release, memory_order_relaxed)) {
            // 有新节点加入，等待它设置 next
            while (node->next == NULL) {
                // 等待下一个节点连接
            }
                }
    }

    if (node->next != NULL) {
        // 唤醒下一个线程
        atomic_store_explicit(&node->next->waiting, false, memory_order_release);
    }
}

#endif // MCS_LOCK_H
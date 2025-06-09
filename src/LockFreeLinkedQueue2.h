#include <iostream>
#include <atomic>
#include <thread>
#include <assert.h>

// 保证 T 应当是 trivial 的
// 基于链表的无界无锁队列（Michael & Scott 算法）
template<typename T>
class LockFreeLinkedQueue {
public:
    // 保证初始化在单线程下完成
    LockFreeLinkedQueue() {
        Node* node = new Node(Empty);
        node->next.store(nullptr, std::memory_order_relaxed);
        head.store(node, std::memory_order_relaxed);
        tail.store(node, std::memory_order_relaxed);
        // 这里 islockfree 是一个额外的标记，可通过 Node 内的 atomic 判断，但此处仅供示例
        islockfree = node->val.is_lock_free();
    }
    ~LockFreeLinkedQueue() {
        T tmp = Empty;
        // 释放所有节点
        while (tryPop(tmp));
        Node* node = head.load(std::memory_order_relaxed);
        if (node != nullptr)
            delete node;
    }
    bool is_lock_free() {
        return islockfree;
    }

    bool isEmpty() { return count.load(std::memory_order_relaxed) == 0; }
    bool isFull() { return false; }

    // push 操作：插入值
    bool push(T val);

    // pop 操作：尝试弹出值，成功返回 true
    bool tryPop(T& val);

    // pop 操作：当队列中无元素时会自旋等待
    T pop();

private:
    struct Node {
        std::atomic<T> val;
        std::atomic<Node*> next;
        Node(T v) : val(v), next(nullptr) {}
    };
    const T Empty = 0;
    std::atomic<int> count{ 0 };  // 计数器，仅供额外调试
    std::atomic<Node*> head;  // 头结点（哨兵节点）
    std::atomic<Node*> tail;  // 尾结点
    bool islockfree;
};


// push 操作：插入值到队列尾部
template<typename T>
bool LockFreeLinkedQueue<T>::push(T val) {
    Node* new_node = new Node(val);
    new_node->next.store(nullptr, std::memory_order_relaxed);
    while (true) {
        Node* last = tail.load(std::memory_order_acquire);
        Node* next = last->next.load(std::memory_order_acquire);
        // 检查 tail 是否依然一致
        if (last == tail.load(std::memory_order_acquire)) {
            if (next == nullptr) { // tail 真正为最后一个节点
                if (last->next.compare_exchange_weak(next, new_node,
                    std::memory_order_release, std::memory_order_relaxed)) {
                    // 尝试更新 tail 指针
                    tail.compare_exchange_weak(last, new_node,
                        std::memory_order_release, std::memory_order_relaxed);
                    count.fetch_add(1, std::memory_order_relaxed);
                    return true;
                }
            }
            else {
                // tail 指针落后，帮助更新 tail
                tail.compare_exchange_weak(last, next,
                    std::memory_order_release, std::memory_order_relaxed);
            }
        }
    }
}


// tryPop 操作：弹出队首值（不删除哨兵节点），失败返回 false
template<typename T>
bool LockFreeLinkedQueue<T>::tryPop(T& val) {
    while (true) {
        Node* first = head.load(std::memory_order_acquire);
        Node* last = tail.load(std::memory_order_acquire);
        Node* next = first->next.load(std::memory_order_acquire);
        if (first == head.load(std::memory_order_acquire)) {
            if (first == last) {
                // 队列为空
                if (next == nullptr)
                    return false;
                // tail 落后，尝试更新
                tail.compare_exchange_weak(last, next,
                    std::memory_order_release, std::memory_order_relaxed);
            }
            else {
                // 读取待弹出值
                val = next->val.load(std::memory_order_acquire);
                if (head.compare_exchange_weak(first, next,
                    std::memory_order_release, std::memory_order_relaxed)) {
                    count.fetch_sub(1, std::memory_order_relaxed);
                    delete first;
                    return true;
                }
            }
        }
    }
}


// pop 操作：自旋直到成功弹出队首值
template<typename T>
T LockFreeLinkedQueue<T>::pop() {
    T result;
    while (!tryPop(result)) {
        std::this_thread::yield();
    }
    return result;
}

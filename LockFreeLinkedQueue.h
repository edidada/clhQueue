#include<iostream>
#include<atomic>
#include<thread>
#include<assert.h>

//保证T应当是trival
//基于链表的无界无锁队列
template<typename T>
class LockFreeLinkedQueue {
public:
    //保证初始化在单线程下完成
    LockFreeLinkedQueue() {
        Node* node = new Node(Empty);
        head.store(node);
        tail.store(node);
        islockfree = node->val.is_lock_free();
    }
    ~LockFreeLinkedQueue() {
        T val = Empty;
        while (tryPop(val));
        Node* node = head.load();
        if (node != NULL)
            delete node;
    }
    bool is_lock_free() {
        return islockfree;
    }

    bool isEmpty() { return count.load() == 0; }
    bool isFull() { return false; }

    //push操作，CAS加tail锁
    bool push(T val);

    //pop操作，CAS加head锁
    bool tryPop(T& val);

    //不建议使用，当队列中无元素时，会自旋
    T pop();

private:
    struct Node {
        std::atomic<T> val;
        std::atomic<Node*>next;
        Node(T val) :val(val) {

        }
    };
    const T Empty = 0;
    std::atomic<int>count = { 0 };  //计数器
    std::atomic<Node*>head;  //头结点
    std::atomic<Node*>tail;   //尾结点
    bool islockfree;
};


//push操作，CAS加tail锁
template<typename T>
bool LockFreeLinkedQueue<T>::push(T val) {
    Node* t = NULL;
    Node* node = new Node(val);
    while (true) {
        //t==NULL，表示tail锁被抢
        if (t == NULL) {
            t = tail.load();
            continue;
        }
        //尝试加tail锁
        if (!tail.compare_exchange_weak(t, NULL))
            continue;
        break;
    }
    t->next.store(node);
    ++count;
    Node* expected = NULL;
    //释放tail锁
    bool flag = tail.compare_exchange_weak(expected, t->next);
    assert(flag);
    return true;
}

//pop操作，CAS加head锁
template<typename T>
bool LockFreeLinkedQueue<T>::tryPop(T& val) {
    Node* h = NULL, * h_next = NULL;
    while (true) {
        //h==NULL，表示head锁被抢
        if (h == NULL) {
            h = head.load();
            continue;
        }
        //尝试加head锁
        if (!head.compare_exchange_weak(h, NULL))
            continue;
        h_next = h->next.load();
        //h->next != NULL 且 count == 0
        //   此时在push函数中数据以及count计数器没有来得及更新，因此进行自旋
        if (h_next != NULL) {
            while (count.load() == 0)
                std::this_thread::yield();  //???
        }
        break;
    }
    Node* expected = NULL;
    Node* desired = h;
    //当h_next==NULL时
    //   表示当前链表为空
    if (h_next != NULL) {
        val = h_next->val;
        delete h;
        desired = h_next;
        --count;
    }
    //CAS head，释放head锁
    bool flag = head.compare_exchange_weak(expected, desired);
    assert(flag);
    return h_next != NULL;
}

//不建议使用，当队列中无元素时，会自旋
template<typename T>
T LockFreeLinkedQueue<T>::pop() {
    Node* h = NULL, * h_next = NULL;
    while (true) {
        //h==NULL，表示head锁被抢
        if (h == NULL) {
            h = head.load();
            continue;
        }
        //尝试加head锁
        if (!head.compare_exchange_weak(h, NULL))
            continue;
        h_next = h->next.load();
        //h->next == NULL
        //   当前队列为空，是否需要解head锁？
        //h->next != NULL 且 count == 0
        //   此时在push函数中数据以及count计数器没有来得及更新，因此进行自旋
        while (h_next == NULL || count.load() == 0) {
            std::this_thread::yield();  //???
            if (h_next == NULL)
                h_next = h->next.load();
        }
        break;
    }
    T val = h_next->val;
    delete h;
    --count;
    Node* expected = NULL;
    Node* desired = h_next;
    //CAS head，释放head锁
    bool flag = head.compare_exchange_weak(expected, desired);
    assert(flag);
    return val;
}

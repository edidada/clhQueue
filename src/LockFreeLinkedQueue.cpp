#include "LockFreeLinkedQueue.h"

#include<vector>
#include<unordered_map>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<iostream>
#include <ctime>
#include <atomic>

using namespace std;

std::mutex mx;
condition_variable cond;
bool running = false;
atomic<int> cnt(0);

//队列
LockFreeLinkedQueue<int> queue;

const int pn = 100, cn = 100;  //生产者/消费者线程数
//每个生产者push batch*num的数据
const int batch = 100;
const int num = 100;
unordered_map<int, int> counts[cn];

//生产者
void produce() {
    {
        std::unique_lock<std::mutex>lock(mx);
        cond.wait(lock, []() {return running; });
    }
    for (int i = 0; i < batch; ++i) {
        for (int j = 1; j <= num; ++j)
            queue.push(j);
    }
    ++cnt;
}

//消费者
void consume(int i) {
    unordered_map<int, int>& count = counts[i];
    {
        std::unique_lock<std::mutex>lock(mx);
        cond.wait(lock, []() {return running; });
    }
    int val;
    while (true) {
        bool flag = queue.tryPop(val);
        if (flag) ++count[val];
        else if (cnt == pn) break;
    }

}
int main() {
    vector<thread>pThreads, cThreads;
    for (int i = 0; i < pn; ++i)
        pThreads.push_back(thread(&produce));
    for (int i = 0; i < cn; ++i) {
        cThreads.push_back(thread(&consume, i));
    }
    auto start = time(NULL);
    {
        std::lock_guard<std::mutex>guard(mx);
        running = true;
        cond.notify_all();
    }
    for (int i = 0; i < pn; ++i) pThreads[i].join();
    for (int i = 0; i < cn; ++i) cThreads[i].join();
    auto end = time(NULL);

    //结果统计
    unordered_map<int, int> res;
    for (auto& count : counts) {
        for (auto& kv : count) {
            res[kv.first] += kv.second;
        }
    }
    int total = 0;
    int failed = 0;
    for (auto& kv : res) {
        total += kv.second;
        cout << kv.first << " " << kv.second << endl;
        failed = !(kv.first > 0 && kv.first <= num && kv.second == batch * pn);
    }
    cout << "is_lock_free: " << (queue.is_lock_free() ? "true" : "false") << endl;
    cout << "consume time: " << end - start << " seconds" << endl;
    cout << "total numbers: " << total << endl;
    cout << "failed: " << failed << endl;
}
# README

跑不起来
https://codeantenna.com/a/0ffPhvoZQG

https://stackoverflow.com/questions/27314485/use-of-deleted-function-error-with-stdatomic-int

跑起来了，结果如下：
```shell
96 10000
95 10000
94 10000
93 10000
18 10000
17 10000
16 10000
15 10000
14 10000
13 10000
12 10000
11 10000
2 10000
99 10000
1 10000
98 10000
7 10000
8 10000
9 10000
10 10000
23 10000
24 10000
25 10000
26 10000
27 10000
28 10000
41 10000
46 10000
45 10000
6 10000
100 10000
3 10000
31 10000
44 10000
5 10000
30 10000
43 10000
4 10000
29 10000
42 10000
97 10000
40 10000
39 10000
38 10000
37 10000
36 10000
35 10000
34 10000
33 10000
32 10000
19 10000
20 10000
21 10000
22 10000
47 10000
48 10000
49 10000
50 10000
51 10000
52 10000
53 10000
54 10000
55 10000
56 10000
57 10000
58 10000
59 10000
60 10000
61 10000
62 10000
63 10000
64 10000
65 10000
66 10000
67 10000
68 10000
69 10000
70 10000
71 10000
72 10000
73 10000
74 10000
75 10000
76 10000
77 10000
78 10000
79 10000
80 10000
81 10000
82 10000
83 10000
84 10000
85 10000
86 10000
87 10000
88 10000
89 10000
90 10000
91 10000
92 10000
is_lock_free: true
consume time: 10 seconds
total numbers: 1000000
failed: 0
```

## win

```shell
D:\develops\git\github\cpp\clhQueue\cmake-build-debug\clhQueue.exe
Process finished with exit code -1073741819 (0xC0000005)
```

如果是自己写的 Lock-Free 队列，请参考标准实现（如 boost 或 Facebook 的 folly）


Exception 0xc0000005 encountered at address 0x7ff6bd7acd6a: Access violation reading location 0xffffffffffffffff
这个错误表示程序在尝试 读取一个无效内存地址 0xFFFFFFFFFFFFFFFF（即 -1 的 64 位整数），这是典型的 野指针或空指针访问问题。

当然可以！下面是将你上面的 **Test-and-Set (TAS) 自旋锁示例代码**整理为一个完整的 C/C++ 项目，并提供：

- ✅ 项目结构命名建议（英文）
- ✅ `CMakeLists.txt` 文件（支持 GCC/Clang/MSVC 编译）
- ✅ 支持 Linux 和 Windows（MSYS2 / MinGW / Visual Studio）

---

## 📁 项目结构建议

```
tas-spinlock-demo/
├── CMakeLists.txt
├── src/
│   └── main.c
└── include/
    └── spinlock.h
```

---

## 📄 文件内容说明

### 1. `include/spinlock.h`

```c
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
```

---

### 2. `src/main.c`

```c
#include "spinlock.h"
#include <pthread.h>
#include <stdio.h>

spinlock_t lock = 0;
int shared_counter = 0;

void* thread_func(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 100000; ++i) {
        spin_lock(&lock);
        ++shared_counter;
        spin_unlock(&lock);
    }
    printf("Thread %d done.\n", id);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    pthread_create(&t1, NULL, thread_func, &id1);
    pthread_create(&t2, NULL, thread_func, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter value: %d\n", shared_counter);
    return 0;
}
```

---

### 3. `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.14)
project(tas_spinlock_demo C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加源文件
add_executable(tas_spinlock_demo src/main.c)

# 链接 pthread 库（Linux）
if(UNIX)
    target_link_libraries(tas_spinlock_demo pthread)
endif()

# Windows 下如果使用 MinGW/MSYS2 一般不需要额外链接
```

---

## 🛠️ 编译方法（跨平台）

### Linux/macOS

```bash
mkdir build && cd build
cmake ..
make
./tas_spinlock_demo
```

### Windows（MSYS2 / MinGW）

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
tas_spinlock_demo.exe
```

### Windows（Visual Studio）

如果你使用的是 Visual Studio：

```bash
cmake -G "Visual Studio 17 2022" ..
```

然后打开生成的 `.sln` 文件即可。

---

## ✅ 总结一句话

> 我为你整理了一个完整的 TAS 自旋锁项目模板，包含清晰的英文命名结构和跨平台 CMake 构建脚本，适用于学习、教学或小型并发库开发。

---

如果你希望我帮你：

- ✅ 扩展为 TTAS 锁、CLH 锁、MCS 锁实现
- ✅ 添加性能测试工具（如 perf、time）
- ✅ 构建为共享库（DLL / .so）供其他项目调用

当然可以！下面是完整的 **Test-and-Test-and-Set (TTAS)** 自旋锁实现，以及配套的 **CMake 项目结构** 和编译脚本。这个方案相比原始的 TAS（Test-and-Set）进一步优化了自旋阶段的行为，在真正调用原子操作前先进行一次非原子检查，减少缓存一致性流量。

---

## 📁 项目结构建议

```
ttas-spinlock-demo/
├── CMakeLists.txt
├── src/
│   └── main.c
└── include/
    └── ttas_spinlock.h
```

---

## 🧠 TTAS 原理简介

### Test-and-Set (TAS) 的问题：

- 所有线程都争抢同一个共享变量，频繁使用原子指令会引发大量缓存一致性流量。

### Test-and-Test-and-Set (TTAS) 的改进：

- 第一步：线程先进行一个普通的、非原子读取，判断锁是否被占用。
- 第二步：如果锁未被占用，才执行原子操作 `test_and_set`。
- 这样减少了对共享变量的原子访问频率，从而降低总线压力和缓存一致性开销。

---

## 📄 文件内容说明

### 1. `include/ttas_spinlock.h`

```c
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
```

---

### 2. `src/main.c`

```c
#include "ttas_spinlock.h"
#include <stdio.h>
#include <pthread.h>

ttas_lock_t lock = 0;
int shared_counter = 0;

void* thread_func(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 100000; ++i) {
        ttas_lock(&lock);
        ++shared_counter;
        ttas_unlock(&lock);
    }
    printf("Thread %d done.\n", id);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    pthread_create(&t1, NULL, thread_func, &id1);
    pthread_create(&t2, NULL, thread_func, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter value: %d\n", shared_counter);
    return 0;
}
```

---

### 3. `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.14)
project(ttas_spinlock_demo C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(ttas_spinlock_demo src/main.c)

# 链接 pthread 库（Linux/macOS）
if(UNIX)
    target_link_libraries(ttas_spinlock_demo pthread)
endif()

# Windows 下使用 MinGW/MSYS2 一般不需要额外链接
```

---

## 🛠️ 编译与运行（跨平台）

### Linux / macOS

```bash
mkdir build && cd build
cmake ..
make
./ttas_spinlock_demo
```

### Windows（MSYS2 / MinGW）

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
ttas_spinlock_demo.exe
```

### Windows（Visual Studio）

```bash
cmake -G "Visual Studio 17 2022" ..
```

然后打开生成的 `.sln` 文件即可构建和调试。

---

## ✅ 总结一句话

> 我为你提供了一个完整的 **TTAS 自旋锁**项目模板，包含清晰的英文命名结构和跨平台 CMake 构建系统，适合用于并发编程教学或底层同步机制研究。

---

如果你希望我帮你：

- ✅ 实现 CLH 锁、MCS 锁、Ticket Lock 等现代队列锁
- ✅ 对比不同锁的性能（使用 perf 工具）
- ✅ 封装为共享库（DLL/.so），供其他项目调用

当然可以！下面是一个完整的 **Peterson 锁（Peterson's Algorithm）** 的实现，适用于两个线程之间的互斥访问，并配有清晰的项目结构和 `CMakeLists.txt` 文件，方便你使用 CMake 构建整个项目。

---

## 📁 项目结构建议

```
peterson-lock-demo/
├── CMakeLists.txt
├── src/
│   └── main.c
└── include/
    └── peterson_lock.h
```

---

## 🧠 Peterson 锁简介

Peterson 锁是 Leslie Lamport 提出的一种用于两个进程或线程之间互斥访问临界区的软件算法。它不依赖任何硬件原子指令，仅通过共享变量和顺序一致性来保证互斥性和**无饥饿性（fairness）**。

### 核心变量：

```c
int turn;            // 决定哪个线程等待
bool flag[2];        // 每个线程是否想进入临界区
```

### 算法特点：

- 适用于 **两个线程**
- 不需要原子指令（纯软件实现）
- 保证互斥、无死锁、无饥饿
- 在现代编译器/处理器上可能因优化导致失效（需加内存屏障）

---

## 📄 文件内容说明

### 1. `include/peterson_lock.h`

```c
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
```

---

### 2. `src/main.c`

```c
#include "peterson_lock.h"
#include <stdio.h>
#include <pthread.h>

peterson_lock_t lock;
int shared_counter = 0;

void* thread_func(void* arg) {
    int thread_id = *(int*)arg;

    for (int i = 0; i < 100000; ++i) {
        peterson_lock(&lock, thread_id);
        ++shared_counter;
        peterson_unlock(&lock, thread_id);
    }

    printf("Thread %d done.\n", thread_id);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 0, id2 = 1;

    peterson_lock_init(&lock);

    pthread_create(&t1, NULL, thread_func, &id1);
    pthread_create(&t2, NULL, thread_func, &id2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter value: %d\n", shared_counter);
    return 0;
}
```

---

### 3. `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.14)
project(peterson_lock_demo C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(peterson_lock_demo src/main.c)

# 链接 pthread 库（Linux/macOS）
if(UNIX)
    target_link_libraries(peterson_lock_demo pthread)
endif()

# Windows 下使用 MinGW/MSYS2 一般不需要额外链接
```

---

## 🛠️ 编译与运行（跨平台）

### Linux / macOS

```bash
mkdir build && cd build
cmake ..
make
./peterson_lock_demo
```

### Windows（MSYS2 / MinGW）

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
peterson_lock_demo.exe
```

### Windows（Visual Studio）

```bash
cmake -G "Visual Studio 17 2022" ..
```

然后打开生成的 `.sln` 文件即可构建和调试。

---

## ⚠️ 注意事项

- Peterson 锁只适用于 **两个线程**。
- 在现代 CPU 或优化编译器中，由于指令重排可能导致失败，应使用 `volatile` 或添加内存屏障（如 `__sync_synchronize()` 或 C++ 中的 `std::atomic_thread_fence()`）。
- 实际生产环境中推荐使用更高效的同步机制，如 `pthread_mutex_t`、`CLHLock`、`MCSLock` 等。

---

## ✅ 总结一句话

> 我为你提供了一个完整的 **Peterson 锁**实现，并配有标准的 CMake 构建系统，适合并发编程教学、研究或操作系统课程实验。

---

如果你希望我帮你：

- ✅ 扩展为支持多个线程的 Bounded-Ticket Lock
- ✅ 对比 Peterson 锁与其他自旋锁的性能
- ✅ 将其实现为 C++ 类封装形式
- ✅ 改写为使用内存屏障（memory barrier）增强可靠性

当然可以！下面是一个完整的 **MCS 锁（Mellor-Crummey and Scott Lock）** 的介绍、C 语言实现以及配套的 CMake 构建脚本。MCS 锁是一种高性能的 **基于链表的队列自旋锁（Queue-based Spinlock）**，特别适用于多处理器系统和高并发场景。

---

## 🧠 MCS 锁简介

### 🔹 背景

MCS 锁由 Scott Mellor-Crummey 和 Michael L. Scott 在 1991 年提出，用于解决早期自旋锁（如 TAS、TTAS）在高并发环境下性能差的问题。它通过为每个线程维护一个本地的等待节点（Node），将所有请求锁的线程组织成一个隐式的 **FIFO 队列**，从而减少缓存一致性流量并提高可扩展性。

### 🔹 核心思想

- 每个线程都有自己的节点（Node）
- 所有节点形成一个单向链表
- 当前线程只关注它的前驱节点的状态
- 只有当前节点的前驱释放锁后，该线程才能进入临界区

### 🔹 优点

| 特性 | 描述 |
|------|------|
| ✅ 缓存友好 | 每个线程只访问自己的节点或前驱节点，减少缓存行争用 |
| ✅ 公平性 | FIFO 队列确保公平获取锁 |
| ✅ 可扩展性强 | 支持任意数量的线程 |
| ✅ 局部性好 | 减少跨 CPU 缓存同步 |

---

## 📁 项目结构建议

```
mcs-lock-demo/
├── CMakeLists.txt
├── src/
│   └── main.c
└── include/
    └── mcs_lock.h
```

---

## 📄 文件内容说明

### 1. `include/mcs_lock.h`

```c
#ifndef MCS_LOCK_H
#define MCS_LOCK_H

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
```

---

### 2. `src/main.c`

```c
#include "mcs_lock.h"
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

#define THREAD_COUNT 4

mcs_lock_t lock;
int shared_counter = 0;

void* thread_func(void* arg) {
    mcs_node_t node;
    int id = *(int*)arg;

    for (int i = 0; i < 100000; ++i) {
        mcs_lock(&lock, &node);
        ++shared_counter;
        mcs_unlock(&lock, &node);
    }

    printf("Thread %d done.\n", id);
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    int ids[THREAD_COUNT];

    mcs_lock_init(&lock);

    for (int i = 0; i < THREAD_COUNT; ++i) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }

    for (int i = 0; i < THREAD_COUNT; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("Final counter value: %d\n", shared_counter);
    return 0;
}
```

---

### 3. `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.14)
project(mcs_lock_demo C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(mcs_lock_demo src/main.c)

# 链接 pthread 库（Linux/macOS）
if(UNIX)
    target_link_libraries(mcs_lock_demo pthread)
endif()

# Windows 下使用 MinGW/MSYS2 一般不需要额外链接
```

---

## 🛠️ 编译与运行（跨平台）

### Linux / macOS

```bash
mkdir build && cd build
cmake ..
make
./mcs_lock_demo
```

### Windows（MSYS2 / MinGW）

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
mcs_lock_demo.exe
```

### Windows（Visual Studio）

```bash
cmake -G "Visual Studio 17 2022" ..
```

然后打开生成的 `.sln` 文件即可构建和调试。

---

## ⚠️ 注意事项

- MCS 锁非常适合高并发、多核系统的场景。
- 每个线程必须有自己的节点对象（不能共享），否则会破坏正确性。
- 在现代系统中，应使用内存顺序控制（memory_order）来避免编译器/处理器优化导致的问题。
- 实际生产环境中推荐使用更成熟的无锁库（如 libfabric、Boost.Lockfree）或硬件支持的锁机制。

---

## ✅ 总结一句话

> 我为你提供了一个完整的 **MCS 锁**实现，并配有标准的 CMake 构建系统，适合用于操作系统教学、并发编程研究或底层同步机制开发。

---

如果你希望我帮你：

- ✅ 实现 CLH 锁、Ticket Lock、Array-based Lock
- ✅ 使用 C++ 封装为类模板形式
- ✅ 对比不同锁的性能（加 perf 工具分析）
- ✅ 扩展为支持递归锁、读写锁


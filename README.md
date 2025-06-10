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



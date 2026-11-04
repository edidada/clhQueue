cmake_minimum_required(VERSION 3.14)
project(clhQueue)

set(CMAKE_CXX_STANDARD 11)

# 主程序：跨平台（使用 std::thread，所有平台均可构建）
add_executable(clhQueue src/LockFreeLinkedQueue.cpp)

# 下面的锁演示程序依赖 POSIX 线程（pthread.h），仅在类 Unix 平台构建
if(UNIX)
    add_executable(tas_spinlock_demo src/tas_spinlock_demo.c)
    add_executable(ttas_spinlock src/ttas_spinlock.c)
    add_executable(peterson_lock src/peterson_lock.c)
    add_executable(bakery_lock src/bakery_lock.c)
    add_executable(mcs_lock src/mcs_lock.c)

    # 链接 pthread 库（Linux/macOS）
    target_link_libraries(tas_spinlock_demo pthread)
    target_link_libraries(ttas_spinlock pthread)
    target_link_libraries(peterson_lock pthread)
    target_link_libraries(bakery_lock pthread)
    target_link_libraries(mcs_lock pthread)
endif()

# 注册测试：ctest 会实际运行这些程序，验证它们能正常执行
enable_testing()
add_test(NAME clhQueue COMMAND clhQueue)

if(UNIX)
    add_test(NAME tas_spinlock_demo COMMAND tas_spinlock_demo)
    add_test(NAME ttas_spinlock COMMAND ttas_spinlock)
    add_test(NAME peterson_lock COMMAND peterson_lock)
    add_test(NAME bakery_lock COMMAND bakery_lock)
    add_test(NAME mcs_lock COMMAND mcs_lock)
endif()

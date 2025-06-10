cmake_minimum_required(VERSION 3.14)
project(clhQueue)

set(CMAKE_CXX_STANDARD 11)

add_executable(clhQueue src/LockFreeLinkedQueue.cpp)
# 添加源文件
add_executable(tas_spinlock_demo src/main.c)

# 链接 pthread 库（Linux）
if(UNIX)
    target_link_libraries(tas_spinlock_demo pthread)
endif()
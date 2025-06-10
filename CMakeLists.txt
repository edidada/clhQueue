cmake_minimum_required(VERSION 3.14)
project(clhQueue)

set(CMAKE_CXX_STANDARD 11)

add_executable(clhQueue src/LockFreeLinkedQueue.cpp)
# 添加源文件
add_executable(tas_spinlock_demo src/main.c)
add_executable(ttas_spinlock src/ttas_spinlock.c)
add_executable(peterson_lock src/peterson_lock.c)
add_executable(bakery_lock src/bakery_lock.c)
add_executable(mcs_lock src/mcs_lock.c)

# 链接 pthread 库（Linux）
if(UNIX)
    target_link_libraries(tas_spinlock_demo pthread)
    target_link_libraries(ttas_spinlock pthread)
    target_link_libraries(peterson_lock pthread)
    target_link_libraries(bakery_lock pthread)
    target_link_libraries(mcs_lock pthread)
endif()
cmake_minimum_required(VERSION 3.14)
project(clhQueue)

set(CMAKE_CXX_STANDARD 11)

add_executable(clhQueue src/LockFreeLinkedQueue.cpp)
#add_executable(clhQueue src/main.cpp src/LockFreeLinkedQueue.cpp)
target_link_libraries(clhQueue pthread)

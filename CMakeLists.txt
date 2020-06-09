cmake_minimum_required(VERSION 3.15)
project(hw3)

set(CMAKE_CXX_STANDARD 14)

add_executable(hw3 main.cpp Barrier.h testListHooks.cpp ThreadSafeList.h HeavyThreadsTest.cpp)

set(CMAKE_CXX_FLAGS -pthread)
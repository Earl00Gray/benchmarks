cmake_minimum_required(VERSION 3.5)

project(example VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES)
file(GLOB_RECURSE HEADRES benchmark.hpp example.hpp)

add_executable(${CMAKE_PROJECT_NAME} main.cpp ${SOURCES} ${HEADERS})

set(COMPILE_FLAG "-Wall -g -pthread")

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${COMPILE_FLAG}")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COMPILE_FLAG}")
cmake_minimum_required(VERSION 3.26)
project(MainApp)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

add_executable(prominence prominence.cpp)

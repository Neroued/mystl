cmake_minimum_required(VERSION 3.20.0)
project(mystl VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # Clang 使用 libc++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -lc++abi")
endif()

include_directories(mystl)
include_directories(test/include)
include_directories(test/container)

add_executable(test test/test.cpp)


# target_compile_options(allocator_compatibility PUBLIC)
# target_compile_options(allocator_performance PUBLIC)
# target_compile_options(vector PUBLIC -O3)
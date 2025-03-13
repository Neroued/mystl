cmake_minimum_required(VERSION 3.20.0)
project(mystl VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include_directories(mystl)

add_executable(allocator_compatibility test/allocator_compatibility.cpp)
add_executable(allocator_performance test/allocator_performance.cpp)
add_executable(vector test/vector.cpp)
add_executable(wrap_iter test/wrap_iter.cpp)
add_executable(reverse_iterator test/reverse_iterator.cpp)

add_executable(meta test/meta.cpp)

# target_compile_options(allocator_compatibility PUBLIC)
# target_compile_options(allocator_performance PUBLIC)
# target_compile_options(vector PUBLIC -O3)
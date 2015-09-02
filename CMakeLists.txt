cmake_minimum_required (VERSION 2.8.11)
project (EasyOpenCL)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include)
add_subdirectory (src)
add_subdirectory (example)
add_subdirectory (kernels)

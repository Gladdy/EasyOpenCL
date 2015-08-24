add_library (EasyOpenCL easyopencl.cpp boundvalue.cpp kernel.cpp errorhandler.cpp)
target_include_directories (EasyOpenCL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenCL REQUIRED)
include_directories(${OPENCL_INCLUDE_DIRS})
target_link_libraries(EasyOpenCL ${OPENCL_LIBRARIES})

message(STATUS "OpenCL found: ${OPENCL_FOUND}")
message(STATUS "OpenCL includes: ${OPENCL_INCLUDE_DIRS}")
message(STATUS "OpenCL CXX includes: ${OPENCL_HAS_CPP_BINDINGS}")
message(STATUS "OpenCL libraries: ${OPENCL_LIBRARIES}")

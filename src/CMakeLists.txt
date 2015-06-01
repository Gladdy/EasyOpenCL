# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.
add_library (EasyOpenCL easyopencl.cpp)
target_include_directories (EasyOpenCL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
find_package(OpenCL REQUIRED)

include_directories(${OPENCL_INCLUDE_DIRS})
target_link_libraries(EasyOpenCL ${OPENCL_LIBRARIES})

message(STATUS "OpenCL found: ${OPENCL_FOUND}")
message(STATUS "OpenCL includes: ${OPENCL_INCLUDE_DIRS}")
message(STATUS "OpenCL CXX includes: ${OPENCL_HAS_CPP_BINDINGS}")
message(STATUS "OpenCL libraries: ${OPENCL_LIBRARIES}")

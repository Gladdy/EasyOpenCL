add_executable (test main.cpp)

target_link_libraries (test LINK_PUBLIC EasyOpenCL)

file(GLOB KERNEL_FUNCTIONS "*.cl")
file(COPY ${KERNEL_FUNCTIONS} DESTINATION ${CMAKE_BINARY_DIR})

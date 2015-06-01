# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
add_executable (test main.cpp)

# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
target_link_libraries (test LINK_PUBLIC EasyOpenCL)

file(GLOB KERNEL_FUNCTIONS "*.cl")
file(COPY ${KERNEL_FUNCTIONS} DESTINATION ${CMAKE_BINARY_DIR})

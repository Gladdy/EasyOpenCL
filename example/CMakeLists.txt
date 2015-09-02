add_executable (main main.cpp)
target_link_libraries (main LINK_PUBLIC EasyOpenCL)

add_executable (simple simple.cpp)
target_link_libraries (simple LINK_PUBLIC EasyOpenCL)

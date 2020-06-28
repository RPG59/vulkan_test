cmake_minimum_required (VERSION 3.8)
project(vulkan_test)
set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp)

target_link_libraries(main glfw3 vulkan-1)

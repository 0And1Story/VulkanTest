cmake_minimum_required(VERSION 3.25)
project(vulkan_test)

set(CMAKE_CXX_STANDARD 23)

include_directories("./include")
link_directories("./lib")

add_executable(vulkan_test src/main.cpp)

# Vulkan SDK
find_package(Vulkan REQUIRED)
target_link_libraries(vulkan_test Vulkan::Vulkan)

# GLFW
target_link_libraries(vulkan_test glfw3)

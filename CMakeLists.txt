cmake_minimum_required(VERSION 3.25)
project(vulkan_test)

set(CMAKE_CXX_STANDARD 23)

include_directories("./include")
link_directories("./lib")

add_executable(vulkan_test src/main.cpp)

# Vulkan SDK
find_package(Vulkan REQUIRED)
link_libraries(vulkan_test Vulkan::Vulkan)

# GLFW
link_libraries(vulkan_test glfw3)

add_subdirectory(toy2d)
target_link_libraries(vulkan_test toy2d)

target_include_directories(vulkan_test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Compile Shader
add_subdirectory(shader)

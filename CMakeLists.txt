cmake_minimum_required(VERSION 3.25)
project(vulkan_test)

set(CMAKE_CXX_STANDARD 23)

include_directories("./include")
link_directories("./lib")

add_executable(vulkan_test src/main.cpp)

# Vulkan SDK
find_package(Vulkan REQUIRED)
link_libraries(Vulkan::Vulkan)

# GLFW
link_libraries(glfw3)

# STB
add_subdirectory(stb)
link_libraries(stb)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(toy2d)
target_link_libraries(vulkan_test toy2d)

# Compile Shader
add_subdirectory(shader)

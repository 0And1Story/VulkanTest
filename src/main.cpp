/**
  * @file   main.cpp
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */
#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    if (!glfwVulkanSupported()) {
        std::cerr << "Vulkan not supported" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "Vulkan is supported!" << std::endl;
    glfwTerminate();
    return 0;
}

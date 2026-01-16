/**
  * @file   main.cpp
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */
#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"

#include "toy2d/toy2d.hpp"

#include <iostream>
#include <vector>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

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

    std::clog << "Vulkan is supported!" << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Toy2D", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // enable Vulkan instance extensions required by GLFW
    uint32_t ext_cnt;
    auto extensions_c = glfwGetRequiredInstanceExtensions(&ext_cnt);
    std::vector<const char*> extensions(extensions_c, extensions_c + ext_cnt);

    std::clog << "Required Vulkan instance extensions:";
    for (const auto& extension : extensions) std::clog << " " << extension;
    std::clog << std::endl;

    toy2d::Init(
        extensions,
        [&](vk::Instance instance) -> vk::SurfaceKHR {
            VkSurfaceKHR surface;
            if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create window surface.");
            }
            return surface;
        },
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    auto& renderer = toy2d::GetRenderer();

    while (!glfwWindowShouldClose(window)) {
//        glfwSwapBuffers(window);
        glfwPollEvents();
        renderer.DrawTriangle();
    }

    toy2d::Quit();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

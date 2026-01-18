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
#include <array>

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

using toy2d::vec2;
static std::array<vec2, 3> triangle_vertices = {
    vec2(0.0, -0.5),
    vec2(0.35, 0.5),
    vec2(-0.35, 0.5)
};
static std::array<vec2, 4> rect_vertices = {
    vec2(-0.5, -0.5),
    vec2(0.5, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
};
static std::array<uint32_t, 6> rect_indices = {
    0, 1, 2,
    2, 3, 0
};
static toy2d::UniformObject ubo {
    .opacity = 1.0f
};
toy2d::Renderer* pRenderer;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        ubo.opacity = 0.5f;
        pRenderer->SetUniformObject(ubo);
    }
    if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
        ubo.opacity = 1.0f;
        pRenderer->SetUniformObject(ubo);
    }
}

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
    pRenderer = &renderer;
    glfwSetKeyCallback(window, key_callback);

    // Draw Triangle:
    // renderer.InitTriangle();
    // renderer.SetTriangle(triangle_vertices);
    // renderer.SetUniformObject(ubo);

    renderer.InitRectangle();
    renderer.SetRectangle(rect_vertices, rect_indices);
    renderer.SetUniformObject(ubo);
//    renderer.SetTexture("resources/texture.png");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // renderer.DrawTriangle();
        renderer.DrawRectangle();
    }

    toy2d::Quit();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

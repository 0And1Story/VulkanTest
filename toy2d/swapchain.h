/**
  * @file   swapchain.h
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

class Swapchain {
public:
    struct SwapchainInfo {
        vk::Extent2D imageExtent;
        uint32_t imageCount;
        vk::SurfaceFormatKHR format;
        vk::SurfaceTransformFlagBitsKHR transform;
        vk::PresentModeKHR present;
    };

    vk::SwapchainKHR swapchain;
    SwapchainInfo info;

public:
    Swapchain(int w, int h);
    ~Swapchain();

    void queryInfo(int w, int h);
};

}

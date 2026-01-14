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
    vk::SwapchainKHR swapchain;

    struct SwapchainInfo {
        vk::Extent2D imageExtent;
        uint32_t imageCount;
        vk::SurfaceFormatKHR format;
        vk::SurfaceTransformFlagBitsKHR transform;
        vk::PresentModeKHR present;
    };

    SwapchainInfo info;
    std::vector<vk::Image> images;
    std::vector<vk::ImageView> imageViews;

public:
    Swapchain(int w, int h);
    ~Swapchain();

    void queryInfo(int w, int h);
    void getImages();
    void createImageViews();
};

}

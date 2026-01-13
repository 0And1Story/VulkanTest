/**
  * @file   context.h
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#pragma once

#include "utility.h"
#include "swapchain.h"

#include "vulkan/vulkan.hpp"

#include <memory>
#include <optional>
#include <cstdint>

namespace toy2d {

class Context {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsQueue;
        std::optional<uint32_t> presentQueue;

        operator bool() const { return graphicsQueue.has_value() && presentQueue.has_value(); }
    };

    vk::Instance instance;
    vk::PhysicalDevice phyDevice;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;
    vk::SurfaceKHR surface;

    std::unique_ptr<Swapchain> swapchain;

    QueueFamilyIndices queueFamilyIndices;

public:
    ~Context();

    static Context& GetInstance();
    static void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc createSurface);
    static void Quit();

    void InitSwapchain(int w, int h);
    void DestroySwapchain();

    void createInstance(const std::vector<const char*>& extensions);
    void pickupPhysicalDevice();
    void createDevice();
    void queryQueueFamilyIndices();
    void getQueues();

private:
    static std::unique_ptr<Context> _instance;

private:
    Context(const std::vector<const char*>& extensions, CreateSurfaceFunc createSurface);
};

}

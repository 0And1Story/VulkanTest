/**
  * @file   context.h
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

#include <memory>
#include <optional>
#include <cstdint>

namespace toy2d {

class Context {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsQueue;
    };

    vk::Instance instance;
    vk::PhysicalDevice phyDevice;
    vk::Device device;
    vk::Queue graphicsQueue;

    QueueFamilyIndices queueFamilyIndices;

public:
    ~Context();

    static Context& GetInstance();
    static void Init();
    static void Quit();

    void createInstance();
    void pickupPhysicalDevice();
    void createDevice();
    void queryQueueFamilyIndices();
    void getQueues();

private:
    static std::unique_ptr<Context> _instance;

private:
    Context();
};

}

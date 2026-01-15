/**
  * @file   renderer.h
  * @author 0And1Story
  * @date   2026-01-15
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

class Renderer {
private:
    vk::CommandPool _cmdPool;
    vk::CommandBuffer _cmdBuf;

    vk::Semaphore _imageAvailable;
    vk::Semaphore _imageDrawFinished;
    vk::Fence _cmdAvailable;

public:
    Renderer();
    ~Renderer();

    void Render();

private:
    void initCommandPool();
    void allocCommandBuffer();
    void createSemaphores();
    void createFences();
};

}

/**
  * @file   renderer.h
  * @author 0And1Story
  * @date   2026-01-15
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

#include <vector>

namespace toy2d {

class Renderer {
private:
    int _maxFlightCount;
    int _curFrame = 0;

    std::vector<vk::CommandBuffer> _cmdBufs;

    std::vector<vk::Semaphore> _imageAvailableSems;
    std::vector<vk::Semaphore> _imageRenderFinishedSems;
    std::vector<vk::Fence> _cmdAvailableFences;

public:
    Renderer(int maxFlightCount = 2);
    ~Renderer();

    void DrawTriangle();

private:
    void allocCommandBuffer();
    void createSemaphores();
    void createFences();
};

}

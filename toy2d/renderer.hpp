/**
  * @file   renderer.hpp
  * @author 0And1Story
  * @date   2026-01-15
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

#include "buffer.hpp"
#include "vertex.hpp"

#include <vector>
#include <memory>

namespace toy2d {

class Renderer {
private:
    int _maxFlightCount;
    int _curFrame = 0;

    std::vector<vk::CommandBuffer> _cmdBufs;

    std::vector<vk::Semaphore> _imageAvailableSems;
    std::vector<vk::Semaphore> _imageRenderFinishedSems;
    std::vector<vk::Fence> _cmdAvailableFences;

    std::unique_ptr<Buffer> _vertexBuffer;

    static constexpr auto clearColor = vk::ClearColorValue(std::array<float,4> {0.1f, 0.1f, 0.1f, 1.0f});

public:
    Renderer(int maxFlightCount = 2);
    ~Renderer();

    void SetTriangle(const std::array<vec2, 3>& vertices);
    void DrawTriangle();

private:
    void allocCommandBuffer();
    void createSemaphores();
    void createFences();
    void createVertexBuffer(size_t size);
    void bufferVertexData(void* data);
};

}

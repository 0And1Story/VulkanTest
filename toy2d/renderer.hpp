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
#include "uniform.hpp"
#include "texture.hpp"

#include <vector>
#include <memory>
#include <functional>

namespace toy2d {

class Renderer {
private:
    int _maxFlightCount;
    int _curFrame = 0;

    std::vector<vk::CommandBuffer> _cmdBufs;

    std::vector<vk::Semaphore> _imageAvailableSems;
    std::vector<vk::Semaphore> _imageRenderFinishedSems;
    std::vector<vk::Fence> _cmdAvailableFences;

    std::unique_ptr<Buffer> _hostVertexBuffer;
    std::unique_ptr<Buffer> _deviceVertexBuffer;
    std::unique_ptr<Buffer> _hostIndexBuffer;
    std::unique_ptr<Buffer> _deviceIndexBuffer;
    std::vector<std::unique_ptr<Buffer>> _uniformBuffers; // use coherent memory for volatile updates

    vk::DescriptorPool _descriptorPool;
    std::vector<vk::DescriptorSet> _descriptorSets;

    std::unique_ptr<Texture> _texture;
    vk::Sampler _sampler;

    static constexpr auto clearColor = vk::ClearColorValue(std::array<float,4> {0.1f, 0.1f, 0.1f, 1.0f});

public:
    Renderer(int maxFlightCount = 2);
    ~Renderer();

    void Render(const std::function<void(vk::CommandBuffer& cmdBuf)>& renderPassFunc);

    void InitTriangle();
    void SetTriangle(const std::array<vec2, 3>& vertices);
    void DrawTriangle();

    void InitRectangle();
    void SetRectangle(const std::array<vec2, 4>& vertices, const std::array<uint32_t, 6>& indices);
    void DrawRectangle();

    void SetUniformObject(const UniformObject& ubo);
    void SetTexture(std::string_view imagePath);

private:
    void allocCommandBuffer();
    void createSemaphores();
    void createFences();

    void createVertexBuffer(size_t size);
    void bufferVertexData(void* data);
    void createIndexBuffer(size_t size);
    void bufferIndexData(void* data);

    void createUniformBuffer(size_t size);
    void bufferUniformData(void* data);
    void createDescriptorPool();
    void allocDescriptorSets();
    void updateDescriptorSets();

    void createSampler();
};

}

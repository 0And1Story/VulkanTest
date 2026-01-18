/**
  * @file   renderer.cpp
  * @author 0And1Story
  * @date   2026-01-15
  * @brief  
  */

#include "renderer.hpp"

#include "context.hpp"
#include "shader.hpp"

namespace toy2d {

Renderer::Renderer(int maxFlightCount) : _maxFlightCount(maxFlightCount) {
    allocCommandBuffer();
    createSemaphores();
    createFences();
    createSampler();
    SetTexture("resources/texture.png");
    createUniformBuffer(sizeof(UniformObject));
    createDescriptorPool();
    allocDescriptorSets();
    updateDescriptorSets();
}

Renderer::~Renderer() {
    auto& device = Context::GetInstance().device;
    auto& cmdMgr = Context::GetInstance().commandManager;
    device.destroySampler(_sampler);
    device.destroyDescriptorPool(_descriptorPool);
    _hostVertexBuffer.reset();
    _deviceVertexBuffer.reset();
    _hostIndexBuffer.reset();
    _deviceIndexBuffer.reset();
    _uniformBuffers.clear();
    for (auto& sem : _imageAvailableSems) device.destroySemaphore(sem);
    for (auto& sem : _imageRenderFinishedSems) device.destroySemaphore(sem);
    for (auto& fence : _cmdAvailableFences) device.destroyFence(fence);
    for (auto& cmdBuf : _cmdBufs) cmdMgr->FreeCommandBuffer(cmdBuf);
}

void Renderer::allocCommandBuffer() {
    auto& cmdMgr = Context::GetInstance().commandManager;
    _cmdBufs = cmdMgr->AllocCommandBuffers(_maxFlightCount);
}

void Renderer::createSemaphores() {
    auto& device = Context::GetInstance().device;
    vk::SemaphoreCreateInfo createInfo;

    _imageAvailableSems.resize(_maxFlightCount);
    for (auto& sem : _imageAvailableSems) sem = device.createSemaphore(createInfo);

    _imageRenderFinishedSems.resize(_maxFlightCount);
    for (auto& sem : _imageRenderFinishedSems) sem = device.createSemaphore(createInfo);
}

void Renderer::createFences() {
    auto& device = Context::GetInstance().device;
    vk::FenceCreateInfo createInfo;
    createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled); // initially signaled

    _cmdAvailableFences.resize(_maxFlightCount);
    for (auto& fence : _cmdAvailableFences) fence = device.createFence(createInfo);
}

void Renderer::createVertexBuffer(size_t size) {
    // Coherent Memory (Simpler):
    // _vertexBuffer.reset(new Buffer(
    //     size,
    //     vk::BufferUsageFlagBits::eVertexBuffer,
    //     vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    // ));

     _hostVertexBuffer.reset(new Buffer(
         size,
         vk::BufferUsageFlagBits::eTransferSrc,
         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
     ));
     _deviceVertexBuffer.reset(new Buffer(
         size,
         vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
         vk::MemoryPropertyFlagBits::eDeviceLocal
     ));
}

void Renderer::bufferVertexData(void* data) {
    auto& device = Context::GetInstance().device;

    // Coherent Memory (Simpler):
    // void* mapped = device.mapMemory(_vertexBuffer->memory, 0, _vertexBuffer->size); {
    //     std::memcpy(mapped, data, _vertexBuffer->size);
    // } device.unmapMemory(_vertexBuffer->memory);

    auto& ctx = Context::GetInstance();

    void* mapped = device.mapMemory(_hostVertexBuffer->memory, 0, _hostVertexBuffer->size); {
        std::memcpy(mapped, data, _hostVertexBuffer->size);
    } device.unmapMemory(_hostVertexBuffer->memory);

    auto cmdBuf = ctx.commandManager->AllocCommandBuffer();
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuf.begin(beginInfo); {
        vk::BufferCopy region;
        region.setSrcOffset(0).setDstOffset(0).setSize(_hostVertexBuffer->size);
        cmdBuf.copyBuffer(_hostVertexBuffer->buffer, _deviceVertexBuffer->buffer, region);
    } cmdBuf.end();

    vk::SubmitInfo submit;
    submit.setCommandBuffers(cmdBuf);
    ctx.graphicsQueue.submit(submit);

    // wait for transfer to finish
    ctx.device.waitIdle();

    ctx.commandManager->FreeCommandBuffer(cmdBuf);
}

void Renderer::createIndexBuffer(size_t size) {
    _hostIndexBuffer.reset(new Buffer(
            size,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    ));
    _deviceIndexBuffer.reset(new Buffer(
            size,
            vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal
    ));
}

void Renderer::bufferIndexData(void* data) {
    auto& ctx = Context::GetInstance();

    void* mapped = ctx.device.mapMemory(_hostIndexBuffer->memory, 0, _hostIndexBuffer->size); {
        std::memcpy(mapped, data, _hostIndexBuffer->size);
    } ctx.device.unmapMemory(_hostIndexBuffer->memory);

    auto cmdBuf = ctx.commandManager->AllocCommandBuffer();
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuf.begin(beginInfo); {
        vk::BufferCopy region;
        region.setSrcOffset(0).setDstOffset(0).setSize(_hostIndexBuffer->size);
        cmdBuf.copyBuffer(_hostIndexBuffer->buffer, _deviceIndexBuffer->buffer, region);
    } cmdBuf.end();

    vk::SubmitInfo submit;
    submit.setCommandBuffers(cmdBuf);
    ctx.graphicsQueue.submit(submit);

    // wait for transfer to finish
    ctx.device.waitIdle();

    ctx.commandManager->FreeCommandBuffer(cmdBuf);
}

void Renderer::createUniformBuffer(size_t size) {
    _uniformBuffers.resize(_maxFlightCount);

    for (auto& buffer : _uniformBuffers) {
        buffer.reset(new Buffer(
            size,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        ));
    }
}

void Renderer::bufferUniformData(void* data) {
    auto& device = Context::GetInstance().device;

    for (auto& buffer : _uniformBuffers) {
        void* mapped = device.mapMemory(buffer->memory, 0, buffer->size); {
            std::memcpy(mapped, data, buffer->size);
        } device.unmapMemory(buffer->memory);
    }
}

void Renderer::createDescriptorPool() {
    vk::DescriptorPoolCreateInfo createInfo;
    std::vector<vk::DescriptorPoolSize> poolSizes(2);

    poolSizes[0]
    .setType(vk::DescriptorType::eUniformBuffer) // for uniform
    .setDescriptorCount(_maxFlightCount);

     poolSizes[1]
     .setType(vk::DescriptorType::eCombinedImageSampler) // for sampler image
     .setDescriptorCount(_maxFlightCount);

    createInfo
    .setMaxSets(_maxFlightCount)
    .setPoolSizes(poolSizes);
    _descriptorPool = Context::GetInstance().device.createDescriptorPool(createInfo);
}

void Renderer::allocDescriptorSets() {
    auto& ctx = Context::GetInstance();

    auto descriptorSetLayout = Shader::GetInstance().getDescriptorSetLayout();
    std::vector<vk::DescriptorSetLayout> layouts(_maxFlightCount, descriptorSetLayout);

    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo
    .setDescriptorPool(_descriptorPool)
    .setDescriptorSetCount(_maxFlightCount)
    .setSetLayouts(layouts);
    _descriptorSets = ctx.device.allocateDescriptorSets(allocInfo);
}

void Renderer::updateDescriptorSets() {
    auto& ctx = Context::GetInstance();
    for (size_t i = 0; i < _descriptorSets.size(); ++i) {
        auto& descriptorSet = _descriptorSets[i];
        std::vector<vk::WriteDescriptorSet> writers(2);
        std::vector<vk::DescriptorBufferInfo> bufferInfos(1);
        std::vector<vk::DescriptorImageInfo> imageInfos(1);

        // uniform
        bufferInfos[0]
        .setBuffer(_uniformBuffers[i]->buffer)
        .setOffset(0)
        .setRange(_uniformBuffers[i]->size);
        writers[0]
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(1)
        .setDstSet(descriptorSet)
        .setDstBinding(0)
        .setDstArrayElement(0)
        .setBufferInfo(bufferInfos[0]);

        // sampler
        imageInfos[0]
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(_texture->view)
        .setSampler(_sampler);
        writers[1]
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(1)
        .setDstSet(descriptorSet)
        .setDstBinding(1)
        .setDstArrayElement(0)
        .setImageInfo(imageInfos[0]);

        ctx.device.updateDescriptorSets(writers, {}); // what is descriptor copies?
    }
}

void Renderer::createSampler() {
    vk::SamplerCreateInfo createInfo;
    createInfo
    .setMagFilter(vk::Filter::eLinear)
    .setMinFilter(vk::Filter::eLinear)
    .setAddressModeU(vk::SamplerAddressMode::eRepeat)
    .setAddressModeV(vk::SamplerAddressMode::eRepeat)
    .setAddressModeW(vk::SamplerAddressMode::eRepeat)
    .setAnisotropyEnable(false)
    .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
    .setUnnormalizedCoordinates(false)
    .setCompareEnable(false)
    .setMipmapMode(vk::SamplerMipmapMode::eLinear);
    _sampler = Context::GetInstance().device.createSampler(createInfo);
}

void Renderer::InitTriangle() {
    createVertexBuffer(sizeof(vec2[3]));
}

void Renderer::SetTriangle(const std::array<vec2, 3>& vertices) {
    bufferVertexData((void*)vertices.data());
}

void Renderer::DrawTriangle() {
    auto& renderProcess = Context::GetInstance().renderProcess;
    auto& _descriptorSet = _descriptorSets[_curFrame];
    Render([&](vk::CommandBuffer& cmdBuf) {
        cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipeline);
        cmdBuf.bindVertexBuffers(0, _deviceVertexBuffer->buffer, {0});
        cmdBuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, renderProcess->layout, 0, _descriptorSet, {});
        cmdBuf.draw(3, 1, 0, 0); // draw one triangle with 3 vertices
    });
}

void Renderer::InitRectangle() {
    createVertexBuffer(sizeof(vec2[4]));
    createIndexBuffer(sizeof(uint32_t[6]));
}

void Renderer::SetRectangle(const std::array<vec2, 4>& vertices, const std::array<uint32_t, 6>& indices) {
    bufferVertexData((void*)vertices.data());
    bufferIndexData((void*)indices.data());
}

void Renderer::DrawRectangle() {
    auto& renderProcess = Context::GetInstance().renderProcess;
    auto& _descriptorSet = _descriptorSets[_curFrame];
    Render([&](vk::CommandBuffer& cmdBuf) {
        cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipeline);
        cmdBuf.bindVertexBuffers(0, _deviceVertexBuffer->buffer, {0});
        cmdBuf.bindIndexBuffer(_deviceIndexBuffer->buffer, 0, vk::IndexType::eUint32);
        cmdBuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, renderProcess->layout, 0, _descriptorSet, {});
        cmdBuf.drawIndexed(6, 1, 0, 0, 0); // draw rectangle with 6 indices
    });
}

void Renderer::SetUniformObject(const toy2d::UniformObject& ubo) {
    bufferUniformData((void*)&ubo);
}

void Renderer::SetTexture(std::string_view imagePath) {
    _texture.reset(new Texture(imagePath));
}

void Renderer::Render(const std::function<void(vk::CommandBuffer&)>& renderPassFunc) {
    auto& ctx = Context::GetInstance();
    auto& device = ctx.device;
    auto& swapchain = ctx.swapchain;
    auto& renderProcess = ctx.renderProcess;

    /*
     * Render steps:
     *   get image -> record cmd buf -> submit -> present
     * Synchronization:
     *   semaphores: get image -> signal imageAvailable -> submit -> signal imageDrawFinished -> present
     *   fences: submit -> signal cmdAvailable -> present -> wait cmdAvailable -> reset cmdAvailable
     */

    auto& _cmdBuf = _cmdBufs[_curFrame];
    auto& _imageAvailable = _imageAvailableSems[_curFrame];
    auto& _cmdAvailable = _cmdAvailableFences[_curFrame];

    // fences
    if (device.waitForFences(_cmdAvailable,
                             true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fence.");
    }
    device.resetFences(_cmdAvailable);

    // acquire next image from swapchain
    auto result = device.acquireNextImageKHR(swapchain->swapchain, std::numeric_limits<uint64_t>::max(), _imageAvailable);
    if (result.result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to acquire next swapchain image.");
    }
    auto imageIndex = result.value;

    // reset command buffer
    _cmdBuf.reset();

    // record command buffer
    vk::CommandBufferBeginInfo cmdBufBegin;
    cmdBufBegin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit); // only used once
    _cmdBuf.begin(cmdBufBegin); {
        vk::RenderPassBeginInfo renderPassBegin;

        vk::Rect2D area({0, 0}, swapchain->info.imageExtent);
        vk::ClearValue clearValue;
        clearValue.color = Renderer::clearColor;
        renderPassBegin
        .setRenderPass(renderProcess->renderPass)
        .setFramebuffer(swapchain->framebuffers[imageIndex])
        .setRenderArea(area)
        .setClearValues(clearValue);

        _cmdBuf.beginRenderPass(renderPassBegin, {}); { // what is contents?
            renderPassFunc(_cmdBuf);
        } _cmdBuf.endRenderPass();
    } _cmdBuf.end();

    // !!! current frame <-> image index
    auto& _imageRenderFinished = _imageRenderFinishedSems[imageIndex];

    // submit
    vk::SubmitInfo submit;
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit
    .setCommandBuffers(_cmdBuf)
    .setWaitSemaphores(_imageAvailable)
    .setSignalSemaphores(_imageRenderFinished)
    .setWaitDstStageMask(waitStage);
    ctx.graphicsQueue.submit(submit, _cmdAvailable);

    // present
    vk::PresentInfoKHR present;
    present
    .setImageIndices(imageIndex)
    .setSwapchains(swapchain->swapchain)
    .setWaitSemaphores(_imageRenderFinished);
    if (ctx.presentQueue.presentKHR(present) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swapchain image.");
    }

    // in flight
    _curFrame = (_curFrame + 1) % _maxFlightCount;
}

}

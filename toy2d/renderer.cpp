/**
  * @file   renderer.cpp
  * @author 0And1Story
  * @date   2026-01-15
  * @brief  
  */

#include "renderer.hpp"

#include "context.hpp"

namespace toy2d {

Renderer::Renderer(int maxFlightCount) : _maxFlightCount(maxFlightCount) {
    allocCommandBuffer();
    createSemaphores();
    createFences();
}

Renderer::~Renderer() {
    auto& device = Context::GetInstance().device;
    auto& cmdMgr = Context::GetInstance().commandManager;
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
    _vertexBuffer.reset(new Buffer(
        size,
        vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    ));
}

void Renderer::bufferVertexData(void* data) {
    auto& device = Context::GetInstance().device;
    void* mapped = device.mapMemory(_vertexBuffer->memory, 0, _vertexBuffer->size); {
        std::memcpy(mapped, data, _vertexBuffer->size);
    } device.unmapMemory(_vertexBuffer->memory);
}

void Renderer::SetTriangle(const std::array<vec2, 3>& vertices) {
    createVertexBuffer(sizeof(vertices));
    bufferVertexData((void*)vertices.data());
}

void Renderer::DrawTriangle() {
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
            _cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipeline);
            _cmdBuf.bindVertexBuffers(0, _vertexBuffer->buffer, {0});
            _cmdBuf.draw(3, 1, 0, 0); // draw one triangle with 3 vertices
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

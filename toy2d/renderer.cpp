/**
  * @file   renderer.cpp
  * @author 0And1Story
  * @date   2026-01-15
  * @brief  
  */

#include "renderer.h"

#include "context.h"

namespace toy2d {

Renderer::Renderer() {
    initCommandPool();
    allocCommandBuffer();
    createSemaphores();
    createFences();
}

Renderer::~Renderer() {
    auto& device = Context::GetInstance().device;
    device.destroySemaphore(_imageAvailable);
    device.destroySemaphore(_imageDrawFinished);
    device.destroyFence(_cmdAvailable);
    device.freeCommandBuffers(_cmdPool, _cmdBuf);
    device.destroyCommandPool(_cmdPool);
}

void Renderer::initCommandPool() {
    vk::CommandPoolCreateInfo createInfo;
    createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer); // able to reset cmd buf respectively
    _cmdPool = Context::GetInstance().device.createCommandPool(createInfo);
}

void Renderer::allocCommandBuffer() {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo
    .setCommandPool(_cmdPool)
    .setCommandBufferCount(1)
    .setLevel(vk::CommandBufferLevel::ePrimary); // executable directly
    _cmdBuf = Context::GetInstance().device.allocateCommandBuffers(allocInfo)[0]; // only one
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
        clearValue.color = vk::ClearColorValue(std::array<float,4> {0.1f, 0.1f, 0.1f, 1.0f});
        renderPassBegin
        .setRenderPass(renderProcess->renderPass)
        .setFramebuffer(swapchain->framebuffers[imageIndex])
        .setRenderArea(area)
        .setClearValues(clearValue);

        _cmdBuf.beginRenderPass(renderPassBegin, {}); { // what is contents?
            _cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipeline);
            _cmdBuf.draw(3, 1, 0, 0); // draw one triangle with 3 vertices
        } _cmdBuf.endRenderPass();
    } _cmdBuf.end();

    // submit
    vk::SubmitInfo submit;
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit
    .setCommandBuffers(_cmdBuf)
    .setWaitSemaphores(_imageAvailable)
    .setSignalSemaphores(_imageDrawFinished)
    .setWaitDstStageMask(waitStage);
    ctx.graphicsQueue.submit(submit, _cmdAvailable);

    // present
    vk::PresentInfoKHR present;
    present
    .setImageIndices(imageIndex)
    .setSwapchains(swapchain->swapchain)
    .setWaitSemaphores(_imageDrawFinished);
    if (ctx.presentQueue.presentKHR(present) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present swapchain image.");
    }

    // fences
    if (device.waitForFences(_cmdAvailable, true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fence.");
    }
    device.resetFences(_cmdAvailable);
}

void Renderer::createSemaphores() {
    auto& device = Context::GetInstance().device;
    vk::SemaphoreCreateInfo createInfo;
    _imageAvailable = device.createSemaphore(createInfo);
    _imageDrawFinished = device.createSemaphore(createInfo);
}

void Renderer::createFences() {
    auto& device = Context::GetInstance().device;
    vk::FenceCreateInfo createInfo;
    _cmdAvailable = device.createFence(createInfo);
}

}

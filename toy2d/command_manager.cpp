/**
  * @file   command_manager.cpp
  * @author 0And1Story
  * @date   2026-01-15
  * @brief  
  */

#include "command_manager.h"

#include "context.h"

namespace toy2d {

CommandManager::CommandManager() {
    createCommandPool();
}

CommandManager::~CommandManager() {
    auto& device = Context::GetInstance().device;
    device.destroyCommandPool(_pool);
}

void CommandManager::createCommandPool() {
    vk::CommandPoolCreateInfo createInfo;
    createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer); // able to reset cmd buf respectively
    _pool = Context::GetInstance().device.createCommandPool(createInfo);
}

vk::CommandBuffer CommandManager::AllocCommandBuffer() {
    return AllocCommandBuffers(1)[0];
}

std::vector<vk::CommandBuffer> CommandManager::AllocCommandBuffers(uint32_t count) {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo
    .setCommandPool(_pool)
    .setCommandBufferCount(count)
    .setLevel(vk::CommandBufferLevel::ePrimary); // executable directly
    return Context::GetInstance().device.allocateCommandBuffers(allocInfo);
}

void CommandManager::FreeCommandBuffer(const vk::CommandBuffer& cmdBuf) {
    Context::GetInstance().device.freeCommandBuffers(_pool, cmdBuf);
}

void CommandManager::ResetCommandPool() {
    Context::GetInstance().device.resetCommandPool(_pool);
}

}

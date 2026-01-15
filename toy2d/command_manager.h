/**
  * @file   command_manager.h
  * @author 0And1Story
  * @date   2026-01-15
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

#include <vector>

namespace toy2d {

class CommandManager {
private:
    vk::CommandPool _pool;

public:
    CommandManager();
    ~CommandManager();

    std::vector<vk::CommandBuffer> AllocCommandBuffers(uint32_t count);
    vk::CommandBuffer AllocCommandBuffer();
    void FreeCommandBuffer(const vk::CommandBuffer& cmdBuf);
    void ResetCommandPool();

private:
    void createCommandPool();
};

}

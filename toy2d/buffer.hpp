/**
  * @file   buffer.hpp
  * @author 0And1Story
  * @date   2026-01-16
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

#include <optional>

namespace toy2d {

class Buffer {
public:
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    size_t size;

private:
    struct MemoryInfo {
        size_t size;
        std::optional<uint32_t> memoryTypeIndex;
        vk::BufferUsageFlags usage;
        vk::MemoryPropertyFlags property;
    };

    MemoryInfo memoryInfo;

public:
    Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property);
    ~Buffer();

private:
    void createBuffer();
    void queryMemoryInfo();
    void allocMemory();
    void bindMemoryToBuffer();
};

}

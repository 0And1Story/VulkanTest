/**
  * @file   buffer.cpp
  * @author 0And1Story
  * @date   2026-01-16
  * @brief  
  */

#include "buffer.hpp"

#include "context.hpp"

namespace toy2d {

Buffer::Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property)
    : size(size), memoryInfo{ .usage = usage, .property = property } {
    createBuffer();
    queryMemoryInfo();
    allocMemory();
    bindMemoryToBuffer();
}

Buffer::~Buffer() {
    auto& device = Context::GetInstance().device;
    device.freeMemory(memory);
    device.destroyBuffer(buffer);
}

void Buffer::createBuffer() {
    vk::BufferCreateInfo createInfo;
    createInfo
    .setSize(size)
    .setUsage(memoryInfo.usage)
    .setSharingMode(vk::SharingMode::eExclusive);
    buffer = Context::GetInstance().device.createBuffer(createInfo);
}

void Buffer::queryMemoryInfo() {
    auto& ctx = Context::GetInstance();
    auto requirements = ctx.device.getBufferMemoryRequirements(buffer);
    memoryInfo.size = requirements.size;

    auto properties = ctx.phyDevice.getMemoryProperties();
    for (size_t i = 0; i < properties.memoryTypeCount; ++i) {
        if ((requirements.memoryTypeBits & (1 << i)) &&
            (properties.memoryTypes[i].propertyFlags & memoryInfo.property)) {
            memoryInfo.memoryTypeIndex = i;
            break;
        }
    }

    if (!memoryInfo.memoryTypeIndex.has_value()) {
        throw std::runtime_error("Failed to find suitable memory type for buffer.");
    }
}

void Buffer::allocMemory() {
    vk::MemoryAllocateInfo allocInfo;
    allocInfo
    .setAllocationSize(memoryInfo.size)
    .setMemoryTypeIndex(memoryInfo.memoryTypeIndex.value());
    memory = Context::GetInstance().device.allocateMemory(allocInfo);
}

void Buffer::bindMemoryToBuffer() {
    Context::GetInstance().device.bindBufferMemory(buffer, memory, 0);
}

}

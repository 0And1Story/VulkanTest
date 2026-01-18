/**
  * @file   texture.cpp
  * @author 0And1Story
  * @date   2026-01-18
  * @brief  
  */

#include "texture.hpp"

#include "stb/stb_image.h"

#include "context.hpp"

#include <memory>
#include <format>

namespace toy2d {

Texture::Texture(std::string_view imagePath) {
    int w, h, channel;
    stbi_uc* pixels = stbi_load(imagePath.data(), &w, &h, &channel, STBI_rgb_alpha);
    size_t size = w * h * 4;

    if (!pixels) {
        std::cerr << "Failed to load texture image: " << imagePath << std::endl;
        throw std::runtime_error("Failed to load texture image.");
    }

    std::unique_ptr<Buffer> buffer = std::make_unique<Buffer>(
        size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    );

    auto& device = Context::GetInstance().device;
    void* mapped = device.mapMemory(buffer->memory, 0, size); {
        std::memcpy(mapped, pixels, size);
    } device.unmapMemory(buffer->memory);

    createImage(w, h);
    allocMemory();
    device.bindImageMemory(image, memory, 0);

    transitionImageLayoutFromUndefinedToDst();
    transformDataToImage(*buffer, w, h);
    transitionImageLayoutFromDstToShaderReadOnly();

    createImageView();

    stbi_image_free(pixels);
}

Texture::~Texture() {
    auto& device = Context::GetInstance().device;
    device.destroyImageView(view);
    device.freeMemory(memory);
    device.destroyImage(image);
}

void Texture::createImage(uint32_t w, uint32_t h) {
    vk::ImageCreateInfo createInfo;
    createInfo
    .setImageType(vk::ImageType::e2D)
    .setArrayLayers(1)
    .setMipLevels(1)
    .setExtent({w, h, 1})
    .setFormat(vk::Format::eR8G8B8A8Srgb)
    .setTiling(vk::ImageTiling::eOptimal)
    .setInitialLayout(vk::ImageLayout::eUndefined)
    .setUsage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst)
    .setSamples(vk::SampleCountFlagBits::e1);
    image = Context::GetInstance().device.createImage(createInfo);
}

void Texture::createImageView() {
    vk::ImageViewCreateInfo createInfo;

    vk::ComponentMapping mapping;
    vk::ImageSubresourceRange range;
    range
    .setLayerCount(1)
    .setBaseArrayLayer(0)
    .setAspectMask(vk::ImageAspectFlagBits::eColor)
    .setBaseMipLevel(0)
    .setLevelCount(1);

    createInfo
    .setImage(image)
    .setFormat(vk::Format::eR8G8B8A8Srgb)
    .setViewType(vk::ImageViewType::e2D)
    .setComponents(mapping)
    .setSubresourceRange(range);

    view = Context::GetInstance().device.createImageView(createInfo);
}

void Texture::allocMemory() {;
    auto& device = Context::GetInstance().device;
    vk::MemoryAllocateInfo allocInfo;

    auto requirements = device.getImageMemoryRequirements(image);
    allocInfo.setAllocationSize(requirements.size);

    auto index = Buffer::QueryMemoryTypeIndex(requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
    if (!index.has_value()) {
        throw std::runtime_error("Failed to find suitable memory type for texture image.");
    }
    allocInfo.setMemoryTypeIndex(index.value());

    memory = device.allocateMemory(allocInfo);
}

void Texture::transitionImageLayoutFromUndefinedToDst() {
    auto& ctx = Context::GetInstance();
    ctx.commandManager->ExecuteCommand(ctx.graphicsQueue, [&](const vk::CommandBuffer& cmdBuf) {
       vk::ImageMemoryBarrier barrier;
       vk::ImageSubresourceRange range;
       range
       .setLayerCount(1)
       .setBaseArrayLayer(0)
       .setLevelCount(1)
       .setBaseMipLevel(0)
       .setAspectMask(vk::ImageAspectFlagBits::eColor);
       barrier
       .setImage(image)
       .setOldLayout(vk::ImageLayout::eUndefined)
       .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
       .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
       .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
       .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
       .setSubresourceRange(range);
       cmdBuf.pipelineBarrier(
           vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,{},
           {}, {}, barrier
       );
    });
}

void Texture::transitionImageLayoutFromDstToShaderReadOnly() {
    auto& ctx = Context::GetInstance();
    ctx.commandManager->ExecuteCommand(ctx.graphicsQueue, [&](const vk::CommandBuffer& cmdBuf) {
        vk::ImageMemoryBarrier barrier;
        vk::ImageSubresourceRange range;
        range
        .setLayerCount(1)
        .setBaseArrayLayer(0)
        .setLevelCount(1)
        .setBaseMipLevel(0)
        .setAspectMask(vk::ImageAspectFlagBits::eColor);
        barrier
        .setImage(image)
        .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
        .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
        .setSubresourceRange(range);
        cmdBuf.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,{},
            {}, {}, barrier
        );
    });
}

void Texture::transformDataToImage(const toy2d::Buffer& buffer, uint32_t w, uint32_t h) {
    auto& ctx = Context::GetInstance();
    ctx.commandManager->ExecuteCommand(ctx.graphicsQueue, [&](const vk::CommandBuffer& cmdBuf) {
       vk::BufferImageCopy region;
       vk::ImageSubresourceLayers subresource;
       subresource
       .setAspectMask(vk::ImageAspectFlagBits::eColor)
       .setBaseArrayLayer(0)
       .setLayerCount(1)
       .setMipLevel(0);
       region
       .setImageSubresource(subresource)
       .setImageExtent({w, h, 1})
       .setBufferImageHeight(0)
       .setBufferRowLength(0)
       .setBufferOffset(0);
       cmdBuf.copyBufferToImage(buffer.buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
    });
}

}

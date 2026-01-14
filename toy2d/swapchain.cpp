/**
  * @file   swapchain.cpp
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#include "swapchain.h"

#include "context.h"

namespace toy2d {

Swapchain::Swapchain(int w, int h) {
    queryInfo(w, h);

    vk::SwapchainCreateInfoKHR createInfo;
    createInfo
    .setClipped(true)
    .setImageArrayLayers(1) // 1 for 2D
    .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
    .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque) // no transparency
    .setSurface(Context::GetInstance().surface)
    .setImageFormat(info.format.format)
    .setImageColorSpace(info.format.colorSpace)
    .setImageExtent(info.imageExtent)
    .setMinImageCount(info.imageCount)
    .setPreTransform(info.transform)
    .setPresentMode(info.present);

    auto& queueFamilyIndices = Context::GetInstance().queueFamilyIndices;
    if (queueFamilyIndices.graphicsQueue == queueFamilyIndices.presentQueue) {
        createInfo
        .setQueueFamilyIndices(queueFamilyIndices.graphicsQueue.value())
        .setImageSharingMode(vk::SharingMode::eExclusive);
    } else {
        std::array indices =  { queueFamilyIndices.graphicsQueue.value(), queueFamilyIndices.presentQueue.value() };
        createInfo
        .setQueueFamilyIndices(indices)
        .setImageSharingMode(vk::SharingMode::eConcurrent);
    }

    swapchain = Context::GetInstance().device.createSwapchainKHR(createInfo);

    getImages();
    createImageViews();
}

Swapchain::~Swapchain() {
    for (auto& view : imageViews) {
        Context::GetInstance().device.destroyImageView(view);
    }
    Context::GetInstance().device.destroySwapchainKHR(swapchain);
}

void Swapchain::queryInfo(int w, int h) {
    auto& phyDevice = Context::GetInstance().phyDevice;
    auto& surface = Context::GetInstance().surface;
    auto formats = phyDevice.getSurfaceFormatsKHR(surface);

    // choose SRGB if possible
    info.format = formats[0];
    for (const auto& format : formats)  {
        if (format.format == vk::Format::eR8G8B8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            info.format = format;
            break;
        }
    }

    auto capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
    // image count = 2 for double buffering
    info.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);
    info.imageExtent.width = std::clamp<uint32_t>(w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    info.imageExtent.height = std::clamp<uint32_t>(h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    info.transform = capabilities.currentTransform;

    // choose mailbox present mode if possible else fifo
    auto presents = phyDevice.getSurfacePresentModesKHR(surface);
    info.present = vk::PresentModeKHR::eFifo;
    for (const auto& present : presents) {
        if (present == vk::PresentModeKHR::eMailbox) {
            info.present = present;
            break;
        }
    }
}

void Swapchain::getImages() {
    images = Context::GetInstance().device.getSwapchainImagesKHR(swapchain);
}

void Swapchain::createImageViews() {
    // create respective image views for each swapchain image
    imageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); ++i) {
        vk::ImageViewCreateInfo createInfo;

        vk::ComponentMapping mapping; // using default as identity mapping
        vk::ImageSubresourceRange range;
        range
        .setBaseMipLevel(0) // no mipmap
        .setLevelCount(1) // only the original image
        .setBaseArrayLayer(0) // only one layer for 2D
        .setLayerCount(1) // only one layer for 2D
        .setAspectMask(vk::ImageAspectFlagBits::eColor); // color target

        createInfo
        .setImage(images[i])
        .setFormat(info.format.format)
        .setViewType(vk::ImageViewType::e2D) // 2D image
        .setComponents(mapping)
        .setSubresourceRange(range);

        imageViews[i] = Context::GetInstance().device.createImageView(createInfo);
    }
}

}

/**
  * @file   context.cpp
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#include "context.h"

#include <iostream>
#include <vector>

#define ENABLE_VALIDATION_LAYER

namespace toy2d {

std::unique_ptr<Context> Context::_instance = nullptr;

void Context::Init() {
    _instance.reset(new Context);
}

void Context::Quit() {}

Context& Context::GetInstance() {
    return *_instance;
}

Context::Context() {
    createInstance();
    pickupPhysicalDevice();
    queryQueueFamilyIndices();
    createDevice();
    getQueues();
}

Context::~Context() noexcept {
    device.destroy();
    instance.destroy();
}

void Context::createInstance() {
    vk::InstanceCreateInfo createInfo;
    std::vector<const char*> layers;

#if !defined(NDEBUG) && false
    std::clog << "Supported Vulkan layers:" << std::endl;
    auto layers = vk::enumerateInstanceLayerProperties();
    for (auto& layer : layers) {
        std::clog << layer.layerName << ": " << layer.description << std::endl;
    }
    std::clog << std::endl;
#endif
// enable validation layer on debug mode
#if !defined(NDEBUG) && defined(ENABLE_VALIDATION_LAYER)
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_API_VERSION_1_4);
    createInfo
    .setPApplicationInfo(&appInfo)
    .setPEnabledLayerNames(layers);

    instance = vk::createInstance(createInfo);
}

void Context::pickupPhysicalDevice() {
    auto devices = instance.enumeratePhysicalDevices();
    if (devices.empty()) throw std::runtime_error("No Vulkan physical device found.");

#if !defined(NDEBUG) && false
    std::clog << "Available physical devices:" << std::endl;
    for (const auto& device : devices) {
        auto properties = device.getProperties();
        std::clog << properties.deviceName << std::endl;
    }
    std::clog << std::endl;
#endif

    // pick up a discrete GPU if available
    phyDevice = devices[0];
    for (const auto& device : devices) {
        auto properties = device.getProperties();
        if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            phyDevice = device;
            break;
        }
    }
    std::clog << "Device: " << phyDevice.getProperties().deviceName << std::endl;
}

void Context::createDevice() {
    vk::DeviceCreateInfo deviceCreateInfo;
    vk::DeviceQueueCreateInfo queueCreateInfo;
    float priorities[] = {1.0f};
    queueCreateInfo
    .setPQueuePriorities(priorities)
    .setQueueCount(1)
    .setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfo);
    device = phyDevice.createDevice(deviceCreateInfo);
}

void Context::queryQueueFamilyIndices() {
    auto properties = phyDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < properties.size(); ++i) {
        const auto& property = properties[i];
        if (property.queueFlags & vk::QueueFlagBits::eGraphics) {
            queueFamilyIndices.graphicsQueue = i;
            break;
        }
    }
}

void Context::getQueues() {
    graphicsQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(), 0);
}

}

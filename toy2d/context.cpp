/**
  * @file   context.cpp
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#include "context.h"

#include <iostream>
#include <utility>
#include <vector>

#define ENABLE_VALIDATION_LAYER

namespace toy2d {

std::unique_ptr<Context> Context::_instance = nullptr;

void Context::Init(const std::vector<const char*>& extensions, CreateSurfaceFunc createSurface) {
    _instance.reset(new Context(extensions, createSurface));
}

void Context::Quit() {}

Context& Context::GetInstance() {
    return *_instance;
}

Context::Context(const std::vector<const char*>& extensions, CreateSurfaceFunc createSurface) {
    createInstance(extensions);
    pickupPhysicalDevice();
    surface = createSurface(instance);
    queryQueueFamilyIndices();
    createDevice();
    getQueues();
}

Context::~Context() noexcept {
    instance.destroySurfaceKHR(surface);
    device.destroy();
    instance.destroy();
}

void Context::InitSwapchain(int w, int h) {
    swapchain.reset(new Swapchain(w, h));
}

void Context::DestroySwapchain() {
    swapchain.reset();
}

void Context::InitRenderProcess(int w, int h) {
    renderProcess.reset(new RenderProcess(w, h));
}

void Context::DestroyRenderProcess() {
    renderProcess.reset();
}

void Context::CreateFramebuffers(int w, int h) {
    swapchain->createFramebuffers(w, h);
}

void Context::InitCommandManager() {
    commandManager.reset(new CommandManager);
}

void Context::DestroyCommandManager() {
    commandManager.reset();
}

void Context::InitRenderer() {
    renderer.reset(new Renderer);
}

void Context::DestroyRenderer() {
    renderer.reset();
}

void Context::createInstance(const std::vector<const char*>& extensions) {
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
#if !defined(NDEBUG) && false
    std::clog << "Valid Vulkan extensions:" << std::endl;
    auto valid_extensions = vk::enumerateInstanceExtensionProperties();
    for (const auto& ext : valid_extensions) {
        std::clog << ext.extensionName << std::endl;
    }
    std::clog << std::endl;
#endif
// enable validation layer on debug mode
#if !defined(NDEBUG) && defined(ENABLE_VALIDATION_LAYER)
    std::clog << "Validation layer is enabled." << std::endl;
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    vk::ApplicationInfo appInfo;
    appInfo
    .setApiVersion(VK_API_VERSION_1_4)
    .setPApplicationName("Toy2D Application")
    .setApplicationVersion(VK_MAKE_VERSION(0, 1, 0))
    .setPEngineName("Toy2D Engine")
    .setEngineVersion(VK_MAKE_VERSION(0, 1, 0));

    createInfo
    .setPApplicationInfo(&appInfo)
    .setPEnabledLayerNames(layers)
    .setPEnabledExtensionNames(extensions);

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
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    float priorities[] = {1.0f};
    std::array extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    // only one queue if graphics queue and present queue are the same
    if (queueFamilyIndices.graphicsQueue == queueFamilyIndices.presentQueue) {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo
        .setPQueuePriorities(priorities)
        .setQueueCount(1)
        .setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());
        queueCreateInfos.push_back(queueCreateInfo);
    } else {
        vk::DeviceQueueCreateInfo graphicsQueueCreateInfo;
        graphicsQueueCreateInfo
        .setPQueuePriorities(priorities)
        .setQueueCount(1)
        .setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());
        queueCreateInfos.push_back(graphicsQueueCreateInfo);
        vk::DeviceQueueCreateInfo presentQueueCreateInfo;
        presentQueueCreateInfo
        .setPQueuePriorities(priorities)
        .setQueueCount(1)
        .setQueueFamilyIndex(queueFamilyIndices.presentQueue.value());
        queueCreateInfos.push_back(presentQueueCreateInfo);
    }

    deviceCreateInfo
    .setQueueCreateInfos(queueCreateInfos)
    .setPEnabledExtensionNames(extensions);

    device = phyDevice.createDevice(deviceCreateInfo);
}

void Context::queryQueueFamilyIndices() {
    auto properties = phyDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < properties.size(); ++i) {
        const auto& property = properties[i];
        if (property.queueFlags & vk::QueueFlagBits::eGraphics) {
            queueFamilyIndices.graphicsQueue = i;
        }
        if (phyDevice.getSurfaceSupportKHR(i, surface)) {
            queueFamilyIndices.presentQueue = i;
        }
        if (queueFamilyIndices) break;
    }
    if (!queueFamilyIndices) throw std::runtime_error("Failed to find required queue families.");
}

void Context::getQueues() {
    graphicsQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(), 0);
    presentQueue = device.getQueue(queueFamilyIndices.presentQueue.value(), 0);
}

}

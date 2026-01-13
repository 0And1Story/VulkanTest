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
    vk::InstanceCreateInfo createInfo;
    std::vector<const char*> layers;

#if !defined(NDEBUG) && false
    auto layers = vk::enumerateInstanceLayerProperties();
    for (auto& layer : layers) {
        std::clog << layer.layerName << ": " << layer.description << std::endl;
    }
#endif
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

Context::~Context() noexcept {
    instance.destroy();
}

}

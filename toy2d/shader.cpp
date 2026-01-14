/**
  * @file   shader.cpp
  * @author 0And1Story
  * @date   2026-01-14
  * @brief  
  */

#include "shader.h"

#include "context.h"

namespace toy2d {

std::unique_ptr<Shader> Shader::_instance = nullptr;

void Shader::Init(const std::string& vertexSource, const std::string& fragmentSource) {
    _instance.reset(new Shader(vertexSource, fragmentSource));
}

void Shader::Quit() {
    _instance.reset();
}

Shader& Shader::GetInstance() {
    return *_instance;
}

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource) {
    vk::ShaderModuleCreateInfo createInfo;

    createInfo
    .setCodeSize(vertexSource.size())
    .setPCode(reinterpret_cast<const uint32_t*>(vertexSource.data()));
    vertexModule = Context::GetInstance().device.createShaderModule(createInfo);

    createInfo
    .setCodeSize(fragmentSource.size())
    .setPCode(reinterpret_cast<const uint32_t*>(fragmentSource.data()));
    fragmentModule = Context::GetInstance().device.createShaderModule(createInfo);
}

Shader::~Shader() noexcept {
    auto& device = Context::GetInstance().device;
    device.destroyShaderModule(vertexModule);
    device.destroyShaderModule(fragmentModule);
}

}
/**
  * @file   shader.cpp
  * @author 0And1Story
  * @date   2026-01-14
  * @brief  
  */

#include "shader.hpp"

#include "context.hpp"

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

    initStages();
}

Shader::~Shader() noexcept {
    auto& device = Context::GetInstance().device;
    device.destroyShaderModule(vertexModule);
    device.destroyShaderModule(fragmentModule);
}

void Shader::initStages() {
    _stages.resize(2);
    _stages[0]
    .setStage(vk::ShaderStageFlagBits::eVertex)
    .setModule(vertexModule)
    .setPName("main");
    _stages[1]
    .setStage(vk::ShaderStageFlagBits::eFragment)
    .setModule(fragmentModule)
    .setPName("main");
}

std::vector<vk::PipelineShaderStageCreateInfo> Shader::getStages() {
    return _stages;
}

}
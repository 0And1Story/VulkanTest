/**
  * @file   shader.hpp
  * @author 0And1Story
  * @date   2026-01-14
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

#include <memory>
#include <string>

namespace toy2d {

class Shader {
public:
    vk::ShaderModule vertexModule;
    vk::ShaderModule fragmentModule;

private:
    static std::unique_ptr<Shader> _instance;
    std::vector<vk::PipelineShaderStageCreateInfo> _stages;
    vk::DescriptorSetLayout _descriptorSetLayout;

public:
    static void Init(const std::string& vertexSource, const std::string& fragmentSource);
    static void Quit();

    static Shader& GetInstance();

    std::vector<vk::PipelineShaderStageCreateInfo> getStages();
    vk::DescriptorSetLayout getDescriptorSetLayout();

    ~Shader();

private:
    Shader(const std::string& vertexSource, const std::string& fragmentSource);
    void initStages();
    void initDescriptorSetLayout();
};

}

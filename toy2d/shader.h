/**
  * @file   shader.h
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

public:
    static void Init(const std::string& vertexSource, const std::string& fragmentSource);
    static void Quit();

    static Shader& GetInstance();

    std::vector<vk::PipelineShaderStageCreateInfo> getStages();

    ~Shader();

private:
    Shader(const std::string& vertexSource, const std::string& fragmentSource);
    void initStages();
};

}

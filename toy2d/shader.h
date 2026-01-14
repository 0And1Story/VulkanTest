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

public:
    static void Init(const std::string& vertexSource, const std::string& fragmentSource);
    static void Quit();

    static Shader& GetInstance();

    ~Shader();

private:
    static std::unique_ptr<Shader> _instance;

    Shader(const std::string& vertexSource, const std::string& fragmentSource);
};

}

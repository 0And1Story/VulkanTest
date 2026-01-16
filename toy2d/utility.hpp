/**
  * @file   utility.hpp
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

#include <functional>
#include <iostream>
#include <fstream>

namespace toy2d {

using CreateSurfaceFunc = std::function<vk::SurfaceKHR(vk::Instance)>;

std::string ReadWholeFile(const std::string& filepath, std::ios::openmode mode = std::ios::ate);
std::string ReadShaderFile(const std::string& filepath);

}

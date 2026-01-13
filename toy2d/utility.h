/**
  * @file   utility.h
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

#include <functional>

namespace toy2d {

using CreateSurfaceFunc = std::function<vk::SurfaceKHR(vk::Instance)>;

}

/**
  * @file   uniform.hpp
  * @author 0And1Story
  * @date   2026-01-17
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

struct UniformObject {
    float opacity;

    static vk::DescriptorSetLayoutBinding getBinding();
};

}

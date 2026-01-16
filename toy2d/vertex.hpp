/**
  * @file   vertex.hpp
  * @author 0And1Story
  * @date   2026-01-16
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

struct vec2 {
    float x, y;

    static vk::VertexInputAttributeDescription getAttribute();
    static vk::VertexInputBindingDescription getBinding();
};

}

/**
  * @file   vertex.cpp
  * @author 0And1Story
  * @date   2026-01-16
  * @brief  
  */

#include "vertex.hpp"

namespace toy2d {

vk::VertexInputAttributeDescription vec2::getAttribute() {
    vk::VertexInputAttributeDescription attr;
    attr
    .setBinding(0)
    .setFormat(vk::Format::eR32G32Sfloat)
    .setLocation(0)
    .setOffset(0);
    return attr;
}

vk::VertexInputBindingDescription vec2::getBinding() {
    vk::VertexInputBindingDescription binding;
    binding
    .setBinding(0)
    .setInputRate(vk::VertexInputRate::eVertex) // per-vertex data
    .setStride(sizeof(vec2));
    return binding;
}

}

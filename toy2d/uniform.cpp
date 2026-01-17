/**
  * @file   uniform.cpp
  * @author 0And1Story
  * @date   2026-01-17
  * @brief  
  */

#include "uniform.hpp"

namespace toy2d {

vk::DescriptorSetLayoutBinding UniformObject::getBinding() {
    vk::DescriptorSetLayoutBinding binding;
    binding
    .setBinding(0)
    .setDescriptorType(vk::DescriptorType::eUniformBuffer)
    .setDescriptorCount(1) // 1+ if array
    .setStageFlags(vk::ShaderStageFlagBits::eFragment);
    return binding;
}

}

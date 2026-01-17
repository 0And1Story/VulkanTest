/**
  * @file   render_process.hpp
  * @author 0And1Story
  * @date   2026-01-14
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

class RenderProcess {
public:
    vk::Pipeline pipeline;
    vk::PipelineLayout layout;
    vk::RenderPass renderPass;

    vk::DescriptorSetLayout descriptorSetLayout; // only one set layout currently

public:
    RenderProcess(int width, int height);
    ~RenderProcess();

    void InitPipeline(int width, int height);
    void InitLayout();
    void InitRenderPass();
};

}

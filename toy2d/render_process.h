/**
  * @file   render_process.h
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

public:
    void InitRenderProcess(int width, int height);
    void InitPipeline(int width, int height);
    void InitLayout();
    void InitRenderPass();
    void DestroyRenderProcess();
};

}

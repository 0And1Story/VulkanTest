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

public:
    void InitPipeline(int width, int height);
    void DestroyPipeline();
};

}

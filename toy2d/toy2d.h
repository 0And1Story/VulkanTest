/**
  * @file   toy2d.h
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#pragma once

#include "utility.h"

#include "vulkan/vulkan.hpp"

#include "renderer.h"

#include <vector>
#include <functional>

namespace toy2d {

void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc createSurface, int w, int h);
void Quit();

Renderer& GetRenderer();

}

/**
  * @file   toy2d.cpp
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#include "toy2d.h"

#include <utility>
#include "context.h"

namespace toy2d {

void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc createSurface, int w, int h) {
    Context::Init(extensions, createSurface);
    Context::GetInstance().InitSwapchain(w, h);
}

void Quit() {
    Context::GetInstance().DestroySwapchain();
    Context::Quit();
}

}

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

void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc createSurface) {
    Context::Init(extensions, createSurface);
}

void Quit() {
    Context::Quit();
}

}

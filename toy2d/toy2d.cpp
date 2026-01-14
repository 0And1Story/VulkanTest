/**
  * @file   toy2d.cpp
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#include "toy2d.h"

#include "utility.h"
#include "context.h"
#include "shader.h"

namespace toy2d {

void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc createSurface, int w, int h) {
    Context::Init(extensions, createSurface);
    Context::GetInstance().InitSwapchain(w, h);
    Shader::Init(
        ReadShaderFile("shader/triangle.vert.spv"),
        ReadShaderFile("shader/triangle.frag.spv")
    );
    Context::GetInstance().renderProcess->InitPipeline(w, h);
}

void Quit() {
    Context::GetInstance().renderProcess->DestroyPipeline();
    Shader::Quit();
    Context::GetInstance().DestroySwapchain();
    Context::Quit();
}

}

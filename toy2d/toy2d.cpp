/**
  * @file   toy2d.cpp
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#include "toy2d.hpp"

#include "utility.hpp"
#include "context.hpp"
#include "shader.hpp"

namespace toy2d {

void Init(const std::vector<const char*>& extensions, CreateSurfaceFunc createSurface, int w, int h) {
    Context::Init(extensions, createSurface);
    auto& ctx = Context::GetInstance();
    ctx.InitSwapchain(w, h);
    Shader::Init(ReadShaderFile("shader/triangle.vert.spv"),ReadShaderFile("shader/triangle.frag.spv"));
    ctx.InitRenderProcess(w, h);
    ctx.CreateFramebuffers(w, h);
    ctx.InitCommandManager();
    ctx.InitRenderer();
}

void Quit() {
    auto& ctx = Context::GetInstance();
    ctx.device.waitIdle();
    ctx.DestroyRenderer();
    ctx.DestroyCommandManager();
    ctx.DestroyRenderProcess();
    Shader::Quit();
    ctx.DestroySwapchain();
    Context::Quit();
}

Renderer& GetRenderer() {
    return *Context::GetInstance().renderer;
}

}

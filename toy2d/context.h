/**
  * @file   context.h
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

#include <memory>

namespace toy2d {

class Context {
public:
    vk::Instance instance;

private:
    static std::unique_ptr<Context> _instance;

public:
    ~Context();

    static Context& GetInstance();
    static void Init();
    static void Quit();

private:
    Context();
};

}

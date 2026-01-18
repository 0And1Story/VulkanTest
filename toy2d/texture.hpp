/**
  * @file   texture.hpp
  * @author 0And1Story
  * @date   2026-01-18
  * @brief  
  */

#pragma once

#include "vulkan/vulkan.hpp"

#include "buffer.hpp"

namespace toy2d {

class Texture {
public:
    vk::Image image;
    vk::ImageView view;
    vk::DeviceMemory memory;

public:
    Texture(std::string_view imagePath);
    ~Texture();

private:
    void createImage(uint32_t w, uint32_t h);
    void createImageView();
    void allocMemory();
    void transitionImageLayoutFromUndefinedToDst();
    void transitionImageLayoutFromDstToShaderReadOnly();
    void transformDataToImage(const Buffer& buffer, uint32_t w, uint32_t h);
};

}

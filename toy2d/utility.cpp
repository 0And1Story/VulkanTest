/**
  * @file   utility.cpp
  * @author 0And1Story
  * @date   2026-01-13
  * @brief  
  */

#include "utility.h"

namespace toy2d {

std::string ReadWholeFile(const std::string& filepath, std::ios::openmode mode) {
    std::ifstream file(filepath, mode | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return {};
    }

    auto size = file.tellg();
    std::string content;
    content.resize(size);
    file.seekg(0);

    file.read(content.data(), size);

    return content;
}

std::string ReadShaderFile(const std::string& filepath) {
    return ReadWholeFile(filepath, std::ios::binary);
}

}

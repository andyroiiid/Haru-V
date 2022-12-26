//
// Created by andyroiiid on 12/22/2022.
//

#include "file/HdrImageFile.h"

#include <stb_image.h>

#include "file/FileSystem.h"

HdrImageFile::HdrImageFile(const std::string &filename) {
    stbi_set_flip_vertically_on_load(true);

    std::string bytes = FileSystem::Read(filename);
    int width = 0, height = 0;
    m_data = stbi_loadf_from_memory(
            reinterpret_cast<const stbi_uc *>(bytes.data()),
            static_cast<int>(bytes.length()),
            &width,
            &height,
            nullptr,
            STBI_rgb_alpha
    );
    m_width = width;
    m_height = height;
}

void HdrImageFile::Release() {
    stbi_image_free(m_data);

    m_width = 0;
    m_height = 0;
    m_data = nullptr;
}

void HdrImageFile::Swap(HdrImageFile &other) noexcept {
    std::swap(m_width, other.m_width);
    std::swap(m_height, other.m_height);
    std::swap(m_data, other.m_data);
}

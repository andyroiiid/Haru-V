//
// Created by andyroiiid on 1/8/2023.
//

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <string>

class SingleTextureMaterial;

class BitmapTextRenderer {
public:
    BitmapTextRenderer(const std::string &fontTexture, const glm::vec2 &charSize);

    [[nodiscard]] const glm::vec2 &GetCharSize() const { return m_charSize; }

    void DrawText(const std::string &text, const glm::vec2 &position, const glm::vec4 &color = {1.0f, 1.0f, 1.0f, 1.0f});

private:
    SingleTextureMaterial *m_fontTexture = nullptr;

    glm::vec2 m_charSize;
};

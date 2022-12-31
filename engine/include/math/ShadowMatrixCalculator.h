//
// Created by andyroiiid on 11/25/2022.
//

#pragma once

#include <array>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class ShadowMatrixCalculator {
public:
    void SetCameraInfo(const glm::mat4 &view, float fov, float aspectRatio) {
        m_view = view;
        m_fov = fov;
        m_aspectRatio = aspectRatio;
    }

    void SetLightDirection(const glm::vec3 &lightDir) {
        m_lightDir = lightDir;
    }

    void SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max);

    [[nodiscard]] glm::mat4 CalcShadowMatrix(float near, float far) const;

private:
    [[nodiscard]] std::array<glm::vec4, 8> GetFrustumCorners(float near, float far) const;

    [[nodiscard]] glm::mat4 GetLightProjection(const std::array<glm::vec4, 8> &frustumCorners, const glm::mat4 &lightView) const;

    glm::mat4 m_view{1.0f};
    float m_fov = 0.0f;
    float m_aspectRatio = 1.0f;

    glm::vec3 m_lightDir{};

    std::array<glm::vec4, 8> m_worldCorners{};
};

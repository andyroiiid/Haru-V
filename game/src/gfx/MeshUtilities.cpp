//
// Created by andyroiiid on 12/21/2022.
//

#include "gfx/MeshUtilities.h"

std::vector<VertexPositionOnly> CreateSkyboxVertices() {
    return {
        VertexPositionOnly{{-1.0f, 1.0f, 1.0f}},
        VertexPositionOnly{{1.0f, 1.0f, 1.0f}},
        VertexPositionOnly{{-1.0f, -1.0f, 1.0f}},
        VertexPositionOnly{{1.0f, -1.0f, 1.0f}},
        VertexPositionOnly{{1.0f, -1.0f, -1.0f}},
        VertexPositionOnly{{1.0f, 1.0f, 1.0f}},
        VertexPositionOnly{{1.0f, 1.0f, -1.0f}},
        VertexPositionOnly{{-1.0f, 1.0f, 1.0f}},
        VertexPositionOnly{{-1.0f, 1.0f, -1.0f}},
        VertexPositionOnly{{-1.0f, -1.0f, 1.0f}},
        VertexPositionOnly{{-1.0f, -1.0f, -1.0f}},
        VertexPositionOnly{{1.0f, -1.0f, -1.0f}},
        VertexPositionOnly{{-1.0f, 1.0f, -1.0f}},
        VertexPositionOnly{{1.0f, 1.0f, -1.0}}};
}

void AppendRectVertices(
    std::vector<VertexCanvas> &vertices,
    const glm::vec2           &minPosition,
    const glm::vec2           &maxPosition,
    const glm::vec2           &minTexCoord,
    const glm::vec2           &maxTexCoord
) {
    const glm::vec2 P00{minPosition.x, minPosition.y};
    const glm::vec2 P01{minPosition.x, maxPosition.y};
    const glm::vec2 P10{maxPosition.x, minPosition.y};
    const glm::vec2 P11{maxPosition.x, maxPosition.y};

    const glm::vec2 UV00{minTexCoord.x, minTexCoord.y};
    const glm::vec2 UV01{minTexCoord.x, maxTexCoord.y};
    const glm::vec2 UV10{maxTexCoord.x, minTexCoord.y};
    const glm::vec2 UV11{maxTexCoord.x, maxTexCoord.y};

    vertices.reserve(vertices.size() + 4);
    vertices.emplace_back(P00, UV00);
    vertices.emplace_back(P10, UV10);
    vertices.emplace_back(P01, UV01);
    vertices.emplace_back(P11, UV11);
}

void AppendBoxVertices(std::vector<VertexBase> &vertices, const glm::vec3 &min, const glm::vec3 &max) {
    const glm::vec3 P000{min.x, min.y, min.z};
    const glm::vec3 P001{min.x, min.y, max.z};
    const glm::vec3 P010{min.x, max.y, min.z};
    const glm::vec3 P011{min.x, max.y, max.z};
    const glm::vec3 P100{max.x, min.y, min.z};
    const glm::vec3 P101{max.x, min.y, max.z};
    const glm::vec3 P110{max.x, max.y, min.z};
    const glm::vec3 P111{max.x, max.y, max.z};

    constexpr glm::vec3 NPX{1, 0, 0};
    constexpr glm::vec3 NNX{-1, 0, 0};
    constexpr glm::vec3 NPY{0, 1, 0};
    constexpr glm::vec3 NNY{0, -1, 0};
    constexpr glm::vec3 NPZ{0, 0, 1};
    constexpr glm::vec3 NNZ{0, 0, -1};

    const float WIDTH  = max.x - min.x;
    const float HEIGHT = max.y - min.y;
    const float DEPTH  = max.z - min.z;

    constexpr glm::vec2 UVX00{0.0f, 0.0f};
    const glm::vec2     UVX01{0.0f, HEIGHT};
    const glm::vec2     UVX10{DEPTH, 0.0f};
    const glm::vec2     UVX11{DEPTH, HEIGHT};

    constexpr glm::vec2 UVY00{0.0f, 0.0f};
    const glm::vec2     UVY01{0.0f, DEPTH};
    const glm::vec2     UVY10{WIDTH, 0.0f};
    const glm::vec2     UVY11{WIDTH, DEPTH};

    constexpr glm::vec2 UVZ00{0.0f, 0.0f};
    const glm::vec2     UVZ01{0.0f, HEIGHT};
    const glm::vec2     UVZ10{WIDTH, 0.0f};
    const glm::vec2     UVZ11{WIDTH, HEIGHT};

    vertices.reserve(vertices.size() + 36);
    // +x
    vertices.emplace_back(P100, NPX, UVX00);
    vertices.emplace_back(P101, NPX, UVX10);
    vertices.emplace_back(P110, NPX, UVX01);
    vertices.emplace_back(P110, NPX, UVX01);
    vertices.emplace_back(P101, NPX, UVX10);
    vertices.emplace_back(P111, NPX, UVX11);
    // -x
    vertices.emplace_back(P001, NNX, UVX00);
    vertices.emplace_back(P000, NNX, UVX10);
    vertices.emplace_back(P011, NNX, UVX01);
    vertices.emplace_back(P011, NNX, UVX01);
    vertices.emplace_back(P000, NNX, UVX10);
    vertices.emplace_back(P010, NNX, UVX11);
    // +y
    vertices.emplace_back(P010, NPY, UVY00);
    vertices.emplace_back(P110, NPY, UVY10);
    vertices.emplace_back(P011, NPY, UVY01);
    vertices.emplace_back(P011, NPY, UVY01);
    vertices.emplace_back(P110, NPY, UVY10);
    vertices.emplace_back(P111, NPY, UVY11);
    // -y
    vertices.emplace_back(P001, NNY, UVY00);
    vertices.emplace_back(P101, NNY, UVY10);
    vertices.emplace_back(P000, NNY, UVY01);
    vertices.emplace_back(P000, NNY, UVY01);
    vertices.emplace_back(P101, NNY, UVY10);
    vertices.emplace_back(P100, NNY, UVY11);
    // +z
    vertices.emplace_back(P101, NPZ, UVZ00);
    vertices.emplace_back(P001, NPZ, UVZ10);
    vertices.emplace_back(P111, NPZ, UVZ01);
    vertices.emplace_back(P111, NPZ, UVZ01);
    vertices.emplace_back(P001, NPZ, UVZ10);
    vertices.emplace_back(P011, NPZ, UVZ11);
    // -z
    vertices.emplace_back(P000, NNZ, UVZ00);
    vertices.emplace_back(P100, NNZ, UVZ10);
    vertices.emplace_back(P010, NNZ, UVZ01);
    vertices.emplace_back(P010, NNZ, UVZ01);
    vertices.emplace_back(P100, NNZ, UVZ10);
    vertices.emplace_back(P110, NNZ, UVZ11);
}

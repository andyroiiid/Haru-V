//
// Created by andyroiiid on 12/28/2022.
//

#include "actors/AWorldSpawn.h"

#include <vulkan/VertexFormats.h>

#include "Globals.h"
#include "gfx/Renderer.h"

AWorldSpawn::AWorldSpawn(const std::vector<MapData::Brush> &brushes) {
    std::map<std::string, std::vector<VertexBase>> textureToVertices;
    for (const auto &brush: brushes) {
        for (const auto &face: brush.Faces) {
            std::vector<VertexBase> &vertices = textureToVertices[face.Texture];
            // triangulate
            for (int k = 1; k < face.Vertices.size() - 1; k++) {
                vertices.emplace_back(face.Vertices[0].Position, face.Normal, face.Vertices[0].TexCoord);
                vertices.emplace_back(face.Vertices[k].Position, face.Normal, face.Vertices[k].TexCoord);
                vertices.emplace_back(face.Vertices[k + 1].Position, face.Normal, face.Vertices[k + 1].TexCoord);
            }
        }
    }
    for (const auto &[texture, vertices]: textureToVertices) {
        m_meshes.emplace_back(g_Renderer->CreateMesh(vertices), g_Renderer->LoadPbrMaterial("materials/" + texture + ".json"));
    }
}

void AWorldSpawn::Draw() {
    const glm::mat4 IDENTITY{1.0f};
    for (const auto &[mesh, material]: m_meshes) {
        g_Renderer->Draw(&mesh, IDENTITY, material);
    }
}

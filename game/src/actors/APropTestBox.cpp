//
// Created by andyroiiid on 12/28/2022.
//

#include "actors/APropTestBox.h"

#include "Globals.h"
#include "gfx/Renderer.h"
#include "gfx/MeshUtilities.h"

APropTestBox::APropTestBox(const std::string &materialName, const glm::vec3 &min, const glm::vec3 &max) {
    std::vector<VertexBase> vertices;
    AppendBoxVertices(vertices, min, max);
    m_mesh = g_Renderer->CreateMesh(vertices);
    m_material = g_Renderer->LoadPbrMaterial(materialName);
}

void APropTestBox::Draw() {
    static const glm::mat4 IDENTITY{1.0f};
    g_Renderer->Draw(&m_mesh, IDENTITY, m_material);
}

//
// Created by andyroiiid on 12/27/2022.
//

#include "actors/APropTestModel.h"

#include "Globals.h"
#include "gfx/Renderer.h"

APropTestModel::APropTestModel(const std::string &meshName, const std::string &materialName, const glm::vec3 &position) {
    m_transform.SetPosition(position);
    m_mesh = g_Renderer->LoadObjMesh(meshName);
    m_material = g_Renderer->LoadPbrMaterial(materialName);
}

void APropTestModel::Draw() {
    g_Renderer->Draw(m_mesh, m_transform.GetMatrix(), m_material);
}

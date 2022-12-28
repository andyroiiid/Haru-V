//
// Created by andyroiiid on 12/27/2022.
//

#include "actors/APropTest.h"

#include "gfx/Renderer.h"

APropTest::APropTest(Renderer *renderer, const std::string &meshName, const std::string &materialName, const glm::vec3 &position) {
    m_transform.SetPosition(position);
    m_mesh = renderer->LoadObjMesh(meshName);
    m_material = renderer->LoadPbrMaterial(materialName);
}

void APropTest::Draw(Renderer *renderer) {
    renderer->Draw(m_mesh, m_transform.GetMatrix(), m_material);
}

//
// Created by andyroiiid on 12/28/2022.
//

#include "map/Brushes.h"

#include <PxRigidActor.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>
#include <physics/PhysicsSystem.h>
#include <vulkan/VertexFormats.h>

#include "Globals.h"
#include "gfx/Renderer.h"

static glm::vec3 CalculateCenter(const std::vector<MapData::Brush> &brushes) {
    float     numVertices = 0;
    glm::vec3 sum{0.0f};
    for (const auto &brush: brushes) {
        for (const auto &vertex: brush.Vertices) {
            numVertices += 1.0f;
            sum.x       += vertex.x;
            sum.y       += vertex.y;
            sum.z       += vertex.z;
        }
    }
    return sum / numVertices;
}

Brushes::Brushes(const std::vector<MapData::Brush> &brushes, PhysicsLayer layer)
    : m_center(CalculateCenter(brushes)) {
    CreateMeshes(brushes);
    CreateColliders(brushes, layer);
}

void Brushes::CreateMeshes(const std::vector<MapData::Brush> &brushes) {
    std::map<std::string, std::vector<VertexBase>> textureToVertices;
    for (const auto &brush: brushes) {
        for (const auto &face: brush.Faces) {
            std::vector<VertexBase> &vertices = textureToVertices[face.Texture];
            // triangulate
            for (int k = 1; k < face.Vertices.size() - 1; k++) {
                vertices.emplace_back(
                    face.Vertices[0].Position - m_center, //
                    face.Normal,
                    face.Vertices[0].TexCoord
                );
                vertices.emplace_back(
                    face.Vertices[k].Position - m_center, //
                    face.Normal,
                    face.Vertices[k].TexCoord
                );
                vertices.emplace_back(
                    face.Vertices[k + 1].Position - m_center, //
                    face.Normal,
                    face.Vertices[k + 1].TexCoord
                );
            }
        }
    }
    for (const auto &[texture, vertices]: textureToVertices) {
        m_meshes.emplace_back(
            g_Renderer->CreateMesh(vertices), //
            g_Renderer->LoadPbrMaterial("materials/" + texture + ".json")
        );
    }
}

void Brushes::CreateColliders(const std::vector<MapData::Brush> &brushes, PhysicsLayer layer) {
    const physx::PxFilterData filterData = PhysicsFilterDataFromLayer(layer);

    m_colliders.reserve(brushes.size());
    m_shapes.reserve(brushes.size());
    for (const auto &brush: brushes) {
        std::vector<physx::PxVec3> collider;
        collider.reserve(brush.Vertices.size());
        for (const glm::vec3 &vertex: brush.Vertices) {
            collider.emplace_back(
                vertex.x - m_center.x, //
                vertex.y - m_center.y,
                vertex.z - m_center.z
            );
        }

        physx::PxConvexMesh *brushCollider = g_PhysicsSystem->CreateConvexMesh(collider.size(), collider.data());
        m_colliders.push_back(brushCollider);

        physx::PxShape *brushShape = g_PhysicsScene->CreateShape(physx::PxConvexMeshGeometry(brushCollider), true);
        brushShape->setQueryFilterData(filterData);
        m_shapes.push_back(brushShape);
    }
}

Brushes::~Brushes() {
    g_Renderer->WaitDeviceIdle();

    for (physx::PxShape *shape: m_shapes) {
        PX_RELEASE(shape)
    }

    for (physx::PxConvexMesh *collider: m_colliders) {
        PX_RELEASE(collider)
    }
}

void Brushes::AttachToRigidActor(physx::PxRigidActor *actor) {
    for (const auto &shape: m_shapes) {
        actor->attachShape(*shape);
    }
}

void Brushes::Draw(const glm::mat4 &model) {
    for (const auto &[mesh, material]: m_meshes) {
        g_Renderer->Draw(&mesh, model, material);
    }
}

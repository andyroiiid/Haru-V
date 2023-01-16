//
// Created by andyroiiid on 12/28/2022.
//

#include "map/Brushes.h"

#include <PxRigidActor.h>
#include <core/Debug.h>
#include <foundation/PxAllocator.h>
#include <physics/PhysicsScene.h>
#include <physics/PhysicsSystem.h>
#include <vulkan/VertexFormats.h>

#include "Globals.h"
#include "gfx/PbrRenderer.h"

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
    if (numVertices == 0.0f) {
        return {0.0f, 0.0f, 0.0f};
    }
    return sum / numVertices;
}

Brushes::Brushes(const std::vector<MapData::Brush> &brushes, BrushType type, PhysicsLayer layer)
    : m_type(type)
    , m_center(CalculateCenter(brushes)) {
    switch (type) {
    case BrushType::Normal:
    case BrushType::Trigger:
        CreateMeshes(brushes);
        CreateColliders(brushes, layer);
        break;
    case BrushType::NoCollision:
        CreateMeshes(brushes);
        break;
    case BrushType::NoMesh:
        CreateColliders(brushes, layer);
        break;
    }
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
        if (texture == "trigger") {
            if (m_type != BrushType::Trigger) {
                DebugWarning("Trigger faces should only be used on trigger brushes!");
            }
        } else {
            if (m_type == BrushType::Trigger) {
                DebugWarning("Trigger brushes should only contain trigger faces!");
            }
        }

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

        physx::PxShape *brushShape = g_PhysicsScene->CreateShape(
            physx::PxConvexMeshGeometry(brushCollider), //
            true,
            m_type == BrushType::Trigger
        );
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

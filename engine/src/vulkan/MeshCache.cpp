//
// Created by andyroiiid on 12/27/2022.
//

#include "vulkan/MeshCache.h"

#include <tiny_obj_loader.h>

#include "core/Debug.h"
#include "file/FileSystem.h"
#include "vulkan/VertexFormats.h"

static std::vector<VertexBase> LoadObj(const std::string &filename) {
    tinyobj::ObjReader reader;

    DebugCheckCritical(
            reader.ParseFromString(FileSystem::Read(filename), ""),
            "Failed to load OBJ file {}: {}",
            filename,
            reader.Error()
    );

    DebugCheck(
            reader.Warning().empty(), "Warning when loading OBJ file {}: {}",
            filename,
            reader.Warning()
    );

    const tinyobj::attrib_t &attrib = reader.GetAttrib();
    const std::vector<tinyobj::real_t> &positions = attrib.vertices;
    const std::vector<tinyobj::real_t> &normals = attrib.normals;
    const std::vector<tinyobj::real_t> &texCoords = attrib.texcoords;

    std::vector<VertexBase> vertices;
    for (const tinyobj::shape_t &shape: reader.GetShapes()) {
        const tinyobj::mesh_t &mesh = shape.mesh;
        DebugInfo("Loading OBJ shape {}.", shape.name);

        size_t i = 0;
        for (const size_t f: mesh.num_face_vertices) {
            DebugCheckCritical(f == 3, "All polygons must be triangle.");

            for (size_t v = 0; v < f; v++) {
                const tinyobj::index_t &index = mesh.indices[i + v];
                DebugCheckCritical(index.normal_index >= 0, "Missing normal data on vertex {}/{}.", f, v);
                DebugCheckCritical(index.texcoord_index >= 0, "Missing texture coordinates data on vertex {}/{}.", f, v);

                // X axis is flipped because Blender uses right-handed coordinates
                vertices.emplace_back(
                        glm::vec3{
                                -positions[3 * index.vertex_index + 0],
                                positions[3 * index.vertex_index + 1],
                                positions[3 * index.vertex_index + 2]
                        },
                        glm::vec3{
                                -normals[3 * index.normal_index + 0],
                                normals[3 * index.normal_index + 1],
                                normals[3 * index.normal_index + 2]
                        },
                        glm::vec2{
                                texCoords[2 * index.texcoord_index + 0],
                                texCoords[2 * index.texcoord_index + 1]
                        }
                );
            }
            i += f;
        }
    }
    return vertices;
}

VulkanMesh *MeshCache::LoadObjMesh(const std::string &filename) {
    auto pair = m_meshes.find(filename);
    if (pair == m_meshes.end()) {
        DebugInfo("Caching OBJ mesh {}.", filename);
        const std::vector<VertexBase> vertices = LoadObj(filename);
        pair = m_meshes.emplace(
                filename,
                VulkanMesh(m_device, vertices.size(), sizeof(VertexBase), vertices.data())
        ).first;
    }
    return &pair->second;
}

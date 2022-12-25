//
// Created by andyroiiid on 12/21/2022.
//

#include "MeshUtilities.h"

#include <tiny_obj_loader.h>

#include "core/Debug.h"
#include "file/FileSystem.h"

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

    const float WIDTH = max.x - min.x;
    const float HEIGHT = max.y - min.y;
    const float DEPTH = max.z - min.z;

    constexpr glm::vec2 UVX00{0.0f, 0.0f};
    const glm::vec2 UVX01{0.0f, HEIGHT};
    const glm::vec2 UVX10{DEPTH, 0.0f};
    const glm::vec2 UVX11{DEPTH, HEIGHT};

    constexpr glm::vec2 UVY00{0.0f, 0.0f};
    const glm::vec2 UVY01{0.0f, DEPTH};
    const glm::vec2 UVY10{WIDTH, 0.0f};
    const glm::vec2 UVY11{WIDTH, DEPTH};

    constexpr glm::vec2 UVZ00{0.0f, 0.0f};
    const glm::vec2 UVZ01{0.0f, HEIGHT};
    const glm::vec2 UVZ10{WIDTH, 0.0f};
    const glm::vec2 UVZ11{WIDTH, HEIGHT};

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

void AppendObjVertices(std::vector<VertexBase> &vertices, const std::string &objFilename) {
    DebugInfo("Loading OBJ file {}.", objFilename);

    tinyobj::ObjReader reader;

    DebugCheckCritical(
            reader.ParseFromString(FileSystem::Read(objFilename), ""),
            "Failed to load OBJ file {}: {}",
            objFilename,
            reader.Error()
    );

    DebugCheck(
            reader.Warning().empty(), "Warning when loading OBJ file {}: {}",
            objFilename,
            reader.Warning()
    );

    const tinyobj::attrib_t &attrib = reader.GetAttrib();
    const std::vector<tinyobj::real_t> &positions = attrib.vertices;
    const std::vector<tinyobj::real_t> &normals = attrib.normals;
    const std::vector<tinyobj::real_t> &texCoords = attrib.texcoords;

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
}

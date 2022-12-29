//
// Created by andyroiiid on 12/28/2022.
//

#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace MapData {
    struct Vertex {
        glm::vec3 Position;
        glm::vec2 TexCoord;
    };

    struct Face {
        std::string Texture;
        glm::vec3 Normal;
        std::vector<Vertex> Vertices;
    };

    struct Brush {
        std::vector<glm::vec3> Vertices;
        std::vector<Face> Faces;
    };

    struct Entity {
        std::map<std::string, std::string> Properties;
        std::vector<Brush> Brushes;
    };

    struct Map {
        std::vector<Entity> Entities;
    };
}

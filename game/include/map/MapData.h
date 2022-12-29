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

        [[nodiscard]] bool GetPropertyString(const std::string &key, std::string &value) const;

        [[nodiscard]] bool GetPropertyInteger(const std::string &key, int &value) const;

        [[nodiscard]] bool GetPropertyFloat(const std::string &key, float &value) const;

        [[nodiscard]] bool GetPropertyColor(const std::string &key, glm::vec3 &value) const;

        [[nodiscard]] bool GetPropertyVector(const std::string &key, glm::vec3 &value) const;

    private:
        [[nodiscard]] const std::string &GetProperty(const std::string &key, const std::string &fallback = {}) const;

        [[nodiscard]] bool GetPropertyVec3(const std::string &key, glm::vec3 &value) const;
    };

    struct Map {
        std::vector<Entity> Entities;
    };
}

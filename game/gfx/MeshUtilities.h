//
// Created by andyroiiid on 12/21/2022.
//

#pragma once

#include <vector>
#include <string>

#include "VertexFormats.h"

void AppendBoxVertices(std::vector<VertexBase> &vertices, const glm::vec3 &min, const glm::vec3 &max);

void AppendObjVertices(std::vector<VertexBase> &vertices, const std::string &objFilename);

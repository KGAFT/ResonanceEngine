//
// Created by kgaft on 8/30/24.
//

#include "Mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


Mesh::Mesh(const std::string &name, uint32_t materialId) : name(name), materialId(materialId) {}

void Mesh::setTransformMatrixKhr(VkTransformMatrixKHR *transformMatrixKhr) {
    Mesh::transformMatrixKhr = transformMatrixKhr;
}
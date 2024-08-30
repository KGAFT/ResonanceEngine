//
// Created by kgaft on 8/30/24.
//

#include "Mesh.hpp"

#include <glm/gtc/matrix_transform.hpp>


Mesh::Mesh(const std::string &name, uint32_t materialId) : name(name), materialId(materialId) {}
glm::mat4 Mesh::calculateWorldMatrix() {
    auto worldMatrix = glm::mat4(1.0f);
    worldMatrix = glm::translate(worldMatrix, position);
    worldMatrix = glm::rotate(worldMatrix, rotation.x, glm::vec3(1, 0, 0));
    worldMatrix = glm::rotate(worldMatrix, rotation.y, glm::vec3(0, 1, 0));
    worldMatrix = glm::rotate(worldMatrix, rotation.z, glm::vec3(0, 0, 1));
    worldMatrix = glm::scale(worldMatrix, scale);
    if(transformMatrixKhr){
        auto vkWorldMatrix = MemoryUtils::glmMat4toTransformMatrixKHR(worldMatrix);
        memcpy(transformMatrixKhr, &vkWorldMatrix, sizeof(VkTransformMatrixKHR));
    }
    return worldMatrix;
}

void Mesh::setTransformMatrixKhr(VkTransformMatrixKHR *transformMatrixKhr) {
    Mesh::transformMatrixKhr = transformMatrixKhr;
}
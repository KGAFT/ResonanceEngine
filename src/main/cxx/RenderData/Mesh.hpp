//
// Created by kgaft on 8/30/24.
//

#ifndef MESH_HPP
#define MESH_HPP

#include <glm/glm.hpp>
#include <glm/common.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include <string>
#define GLM
#include "VulkanLib/MemoryUtils/MemoryUtils.hpp"
#include "VulkanLib/Device/Buffer/UniformBuffer.hpp"

class Mesh {
    friend class RenderObjectsDataManager;
    friend class AssetImporter;

public:
    static VkTransformMatrixKHR glmMat4toTransformMatrixKHR(glm::mat4 matrix) {
        // VkTransformMatrixKHR uses a row-major memory layout, while glm::mat4
        // uses a column-major memory layout. We transpose the matrix so we can
        // memcpy the matrix's data directly.
        auto temp = glm::mat3x4(glm::transpose(matrix));
        VkTransformMatrixKHR out_matrix;
        memcpy(&out_matrix, &temp, sizeof(glm::mat3x4));
        return out_matrix;
    }
public:
    Mesh(const std::string &name, uint32_t materialId);

private:
    std::string name = "";
    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);
    glm::vec3 scale = glm::vec3(1);
    uint32_t materialId;
    size_t indirectOffset = 0;
    VkTransformMatrixKHR* transformMatrixKhr = nullptr;
public:
    glm::mat4 calculateWorldMatrix() {
        auto worldMatrix = glm::mat4(1.0f);
        worldMatrix = glm::translate(worldMatrix, position);
        worldMatrix = glm::rotate(worldMatrix, rotation.x, glm::vec3(1, 0, 0));
        worldMatrix = glm::rotate(worldMatrix, rotation.y, glm::vec3(0, 1, 0));
        worldMatrix = glm::rotate(worldMatrix, rotation.z, glm::vec3(0, 0, 1));
        worldMatrix = glm::scale(worldMatrix, scale);
        /*
        if (transformMatrixKhr!=nullptr) {
            auto vkWorldMatrix = glmMat4toTransformMatrixKHR(worldMatrix);
            memcpy(transformMatrixKhr, &vkWorldMatrix, sizeof(VkTransformMatrixKHR));
        }
        */
        return worldMatrix;
    }




    const glm::vec3 &getPosition() const { return position; }

    void setPosition(const glm::vec3 &position) { Mesh::position = position; }

    const glm::vec3 &getRotation() const { return rotation; }

    void setRotation(const glm::vec3 &rotation) { Mesh::rotation = rotation; }

    const glm::vec3 &getScale() const { return scale; }

    void setScale(const glm::vec3 &scale) { Mesh::scale = scale; }

    const std::string &getName() const { return name; }

    size_t getIndirectOffset() {
        return indirectOffset;
    }
private:
    void setTransformMatrixKhr(VkTransformMatrixKHR *transformMatrixKhr);

};



#endif //MESH_HPP

//
// Created by kgaft on 8/30/24.
//

#ifndef MESH_HPP
#define MESH_HPP

#include <glm/glm.hpp>
#include <string>

#include "VulkanLib/Device/Buffer/UniformBuffer.hpp"

class Mesh {
    friend class RenderObjectsDataManager;
    friend class AssetImporter;
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
    glm::mat4 calculateWorldMatrix();



    const glm::vec3 &getPosition() const { return position; }

    void setPosition(const glm::vec3 &position) { Mesh::position = position; }

    const glm::vec3 &getRotation() const { return rotation; }

    void setRotation(const glm::vec3 &rotation) { Mesh::rotation = rotation; }

    const glm::vec3 &getScale() const { return scale; }

    void setScale(const glm::vec3 &scale) { Mesh::scale = scale; }

    const std::string &getName() const { return name; }
private:
    void setTransformMatrixKhr(VkTransformMatrixKHR *transformMatrixKhr);

};



#endif //MESH_HPP

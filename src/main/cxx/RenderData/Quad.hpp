//
// Created by kgaft on 8/30/24.
//

#ifndef QUAD_HPP
#define QUAD_HPP

#include "VulkanLib/Device/Buffer/IndexBuffer.hpp"
#include "VulkanLib/Device/Buffer/VertexBuffer.hpp"


class QuadMesh {
private:
    static inline std::shared_ptr<IndexBuffer> indexBuffer = std::shared_ptr<IndexBuffer>();
    static inline std::shared_ptr<VertexBuffer> vertexBuffer = std::shared_ptr<VertexBuffer>();
    static inline bool initialized = false;
public:
    static std::pair<std::shared_ptr<IndexBuffer>, std::shared_ptr<VertexBuffer>>
    getQuadMesh(std::shared_ptr<LogicalDevice> device) {
        if (!indexBuffer || !vertexBuffer) {
            float QuadVertices[]{
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

            uint32_t QuadIndices[]{
                0, 1, 2,
                3, 2, 1};
            indexBuffer = std::make_shared<IndexBuffer>(device, QuadIndices, 6, vk::IndexType::eUint32, false);
            vertexBuffer = std::make_shared<VertexBuffer>(device, QuadVertices, 4, sizeof(float) * 5,
                                                          vk::Format::eR32G32B32Sfloat, false);
        }
        return std::pair(indexBuffer, vertexBuffer);
    }
    static void releaseMesh(){
        if(indexBuffer){
            indexBuffer->destroy();
        }
        if(vertexBuffer){
            vertexBuffer->destroy();
        }
    }
};



#endif //QUAD_HPP

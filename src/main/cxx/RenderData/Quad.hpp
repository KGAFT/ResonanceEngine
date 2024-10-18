//
// Created by kgaft on 8/30/24.
//

#ifndef QUAD_HPP
#define QUAD_HPP

#include "VulkanLib/Device/Buffer/IndexBuffer.hpp"
#include "VulkanLib/Device/Buffer/VertexBuffer.hpp"


static inline float quadVerticesXPos[]
{
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f
};
static inline uint32_t quadIndicesXPos[]
{
    3, 2, 1, 0, 3, 1
};

static inline float quadVerticesXNeg[]{
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f
};
static inline uint32_t quadIndicesXNeg[]{
    0, 1, 2, 3, 0, 2
};
static inline float quadVerticesYPos[]{
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f
};
static inline uint32_t quadIndicesYPos[]{
    3, 0, 2, 1, 3, 2
};
static inline float quadVerticesYNeg[]
{
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f
};
static inline uint32_t quadIndicesYNeg[]
{
    3, 1, 2, 0, 3, 2
};
static inline float quadVerticesZPos[]
{
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f
};
static inline uint32_t quadIndicesZPos[]
{
    0, 1, 2, 3, 0, 2
};
static inline float quadVerticesZNeg[]
{
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f
};
static inline uint32_t quadIndicesZNeg[]{
    1, 3, 2, 0, 1, 2
};


using namespace std;
class QuadMesh {

private:
    static inline shared_ptr<IndexBuffer> indexBuffer = shared_ptr<IndexBuffer>();
    static inline shared_ptr<VertexBuffer> vertexBuffer = shared_ptr<VertexBuffer>();
    static inline shared_ptr<vector<pair<shared_ptr<IndexBuffer>, shared_ptr<VertexBuffer>>>> quads = std::make_shared<
        vector<pair<shared_ptr<IndexBuffer>, shared_ptr<VertexBuffer> > > >();
    static inline bool initialized = false;
public:
    static pair<shared_ptr<IndexBuffer>, shared_ptr<VertexBuffer>>
    getQuadMesh(shared_ptr<LogicalDevice> device) {
        if (!indexBuffer || !vertexBuffer) {
            float QuadVertices[]{
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

            uint32_t QuadIndices[]{
                0, 1, 2,
                3, 2, 1};
            indexBuffer = make_shared<IndexBuffer>(device, QuadIndices, 6, vk::IndexType::eUint32, false);
            vertexBuffer = make_shared<VertexBuffer>(device, QuadVertices, 4, sizeof(float) * 5,
                                                          vk::Format::eR32G32B32Sfloat, false);
        }
        return pair(indexBuffer, vertexBuffer);
    }
    static shared_ptr<vector<pair<shared_ptr<IndexBuffer>, shared_ptr<VertexBuffer>>>>
        getQuadMeshes(shared_ptr<LogicalDevice> device) {
        if(!quads->size()) {
            quads->push_back({
                make_shared<IndexBuffer>(device, quadIndicesXPos, 6, vk::IndexType::eUint32, false),
                make_shared<VertexBuffer>(device, quadVerticesXPos,
                             4, sizeof(float)*3, vk::Format::eR32G32B32Sfloat, false)
            });

            quads->push_back({
                make_shared<IndexBuffer>(device, quadIndicesXNeg, 6, vk::IndexType::eUint32, false),
                make_shared<VertexBuffer>(device, quadVerticesXNeg,
                             4, sizeof(float)*3, vk::Format::eR32G32B32Sfloat, false)
            });

            quads->push_back({
                make_shared<IndexBuffer>(device, quadIndicesYPos, 6, vk::IndexType::eUint32, false),
                make_shared<VertexBuffer>(device, quadVerticesYPos,
                             4, sizeof(float)*3, vk::Format::eR32G32B32Sfloat, false)

            });

            quads->push_back({
                make_shared<IndexBuffer>(device, quadIndicesYNeg, 6, vk::IndexType::eUint32, false),
                make_shared<VertexBuffer>(device, quadVerticesYNeg,
                             4, sizeof(float)*3, vk::Format::eR32G32B32Sfloat, false)
            });

            quads->push_back({
                make_shared<IndexBuffer>(device, quadIndicesZPos, 6, vk::IndexType::eUint32, false),
                make_shared<VertexBuffer>(device, quadVerticesZPos,
                             4, sizeof(float)*3, vk::Format::eR32G32B32Sfloat, false)
            });

            quads->push_back({
                make_shared<IndexBuffer>(device, quadIndicesZNeg, 6, vk::IndexType::eUint32, false),
                make_shared<VertexBuffer>(device, quadVerticesZNeg,
                             4, sizeof(float)*3, vk::Format::eR32G32B32Sfloat, false)
            });
        }
        return quads;
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

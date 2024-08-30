//
// Created by kgaft on 8/30/24.
//

#ifndef RENDEROBJECTSDATAMANAGER_HPP
#define RENDEROBJECTSDATAMANAGER_HPP


#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include "Model.hpp"
#include "VulkanLib/Device/Buffer/IndexBuffer.hpp"
#include "VulkanLib/Device/Buffer/IndirectBuffer.hpp"
#include "VulkanLib/Device/Buffer/StorageBuffer.hpp"
#include "VulkanLib/Device/Buffer/VertexBuffer.hpp"
#include "VulkanLib/Device/Descriptors/DescriptorSet.hpp"
#include "VulkanLib/Device/Image/Image.hpp"
#include "VulkanLib/Device/Image/Sampler.hpp"
#include "VulkanLib/Device/LogicalDevice/LogicalDevice.hpp"
#include "VulkanLib/Pipelines/RayTracingPipeline/RayTracingPipelineBuilder.hpp"
#include "VulkanLib/Pipelines/RayTracingPipeline/AccelerationStructures/BottomLevelAccelerationStructure.hpp"
#include "VulkanLib/Pipelines/RayTracingPipeline/AccelerationStructures/TopLevelAccelerationStructure.hpp"
#include "VulkanLib/RenderPipeline/GraphicsRenderPipeline.hpp"


struct Material {
    uint32_t albedoIndex = 0;
    uint32_t normalMapIndex = 0;
    uint32_t metallicMapIndex = 0;
    uint32_t roughnessMapIndex = 0;
    uint32_t aoMapIndex = 0;
    uint32_t emissiveMapIndex = 0;
    uint32_t opacityMapIndex = 0;
};

#define TEXTURES_BUFFER_BINDING 0
#define MATERIALS_BUFFER_BINDING 1
#define RT_AS_INFO_BINDING 2
#define RT_OUTPUT_IMAGE_BINDING 3
#define RT_GEOMETRY_NODE_BINDING 4

struct GeometryNode {
    uint32_t materialIndex;
    uint64_t vertexBufferDeviceAddress;
    uint32_t stepSize;
    uint64_t indexBufferDeviceAddress;


};


class RenderObjectsDataManager {
public:
    RenderObjectsDataManager(std::shared_ptr<LogicalDevice> device, bool rtSupport) : device(device),
                                                                                      rtSupport(rtSupport) {

    }

private:
    std::shared_ptr<LogicalDevice> device;
    std::vector<std::pair<std::shared_ptr<Sampler>, std::shared_ptr<Image>>> textureBuffer;

    std::vector<Material> materialsBuffer;
    std::shared_ptr<StorageBuffer> materialsStorage;
    std::shared_ptr<StorageBuffer> geometryNodesStorage;
    std::shared_ptr<IndexBuffer> batchIndex;
    std::shared_ptr<VertexBuffer> batchVertex;
    std::shared_ptr<IndirectBuffer> indirectBuffer;
    std::shared_ptr<Buffer> transformBuffer;
    bool rtSupport = false;

    std::shared_ptr<vkLibRt::BottomLevelAccelerationStructure> bottomLevelAccelerations;
    std::shared_ptr<vkLibRt::TopLevelAccelerationStructure> topLevelAccelerationStructure;
public:
    uint32_t pushImage(std::shared_ptr<Image> image);

    uint32_t pushMaterial(Material &material);

    void setBatchData(std::shared_ptr<IndexBuffer> iBuffer, std::shared_ptr<VertexBuffer> vertexBuffer,
                      std::shared_ptr<IndirectBuffer> indirectBuffer, std::vector<std::shared_ptr<Model>> &models);

    void makeAccelerationsStructuresForRT(Instance &instance);

    void makeMaterialBuffer();

    void writeInfoAboutAcceleration(std::shared_ptr<DescriptorSet> descriptorSet);

    void writeMaterialsInfos(std::shared_ptr<DescriptorSet> descriptorSet);

    void writeImagesInfos(std::shared_ptr<DescriptorSet> descriptorSet);

    void presetBuilder(std::shared_ptr<vkLibRt::RayTracingPipelineBuilder> builder, vk::ShaderStageFlags flags);

    void presetBuilder(std::shared_ptr<RenderPipelineBuilder> builder, vk::ShaderStageFlags flags);

    std::shared_ptr<IndexBuffer> getBatchIndex() const;

    std::shared_ptr<VertexBuffer> getBatchVertex() const;

    std::shared_ptr<IndirectBuffer> getIndirectBuffer() const;

    std::shared_ptr<Buffer> getTransformBuffer() const;

    std::shared_ptr<vkLibRt::TopLevelAccelerationStructure> getTopLevelAccelerationStructure() const;

    const std::shared_ptr<StorageBuffer> &getGeometryNodesStorage() const;
};



#endif //RENDEROBJECTSDATAMANAGER_HPP

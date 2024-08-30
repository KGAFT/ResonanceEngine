//
// Created by kgaft on 8/30/24.
//

#include "RenderObjectsDataManager.hpp"

void RenderObjectsDataManager::makeMaterialBuffer() {
    materialsStorage = std::make_shared<StorageBuffer>(device, materialsBuffer.size() * sizeof(Material),
                                                       rtSupport ? vk::BufferUsageFlagBits::eShaderDeviceAddress
                                                                 : vk::BufferUsageFlags());
    memcpy(materialsStorage->getMapPoint(), materialsBuffer.data(), materialsBuffer.size() * sizeof(Material));
    materialsBuffer.clear();
}

void
RenderObjectsDataManager::setBatchData(std::shared_ptr<IndexBuffer> iBuffer, std::shared_ptr<VertexBuffer> vertexBuffer,
                                       std::shared_ptr<IndirectBuffer> indirectBuffer,
                                       std::vector<std::shared_ptr<Model>> &models) {
    batchIndex = iBuffer;
    batchVertex = vertexBuffer;
    RenderObjectsDataManager::indirectBuffer = indirectBuffer;
    if (rtSupport) {
        uint32_t meshCount = 0;
        for (const auto &item: models) {
            meshCount += item->getMeshes().size();
        }
        transformBuffer = std::make_shared<Buffer>(
                device, meshCount * sizeof(VkTransformMatrixKHR),
                vk::BufferUsageFlagBits::eShaderDeviceAddress |
                vk::BufferUsageFlagBits::
                eAccelerationStructureBuildInputReadOnlyKHR,
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent);
        uint32_t meshCounter = 0;
        VkTransformMatrixKHR *transformMapPoint;
        transformBuffer->map((void **) (&transformMapPoint), 0, vk::MemoryMapFlags());
        for (const auto &item: models) {
            for (const auto &citem: item->getMeshes()) {
                citem->setTransformMatrixKhr(&transformMapPoint[meshCounter]);
                citem->calculateWorldMatrix();
                meshCounter++;
            }
        }
    }
}

void RenderObjectsDataManager::makeAccelerationsStructuresForRT(Instance &instance) {
    auto *indirectMapPoint = (vk::DrawIndexedIndirectCommand *) (indirectBuffer->getMapPoint());
    bottomLevelAccelerations = std::make_shared<vkLibRt::BottomLevelAccelerationStructure>(device, instance);
    std::vector<GeometryNode> geometryNodes;
    vk::DeviceAddress vertexAddress = batchVertex->getBufferAddress(instance.getDynamicLoader());
    vk::DeviceAddress indexAddress = batchIndex->getBufferAddress(instance.getDynamicLoader());
    for (uint32_t i = 0; i < indirectBuffer->getStructuresAmount(); i++) {
        size_t vertexEnd = (i == indirectBuffer->getStructuresAmount() - 1) ? (batchVertex->getVerticesAmount() -
                                                                               indirectMapPoint[i].vertexOffset) :
                           (indirectMapPoint[i + 1].vertexOffset - indirectMapPoint[i].vertexOffset);

        bottomLevelAccelerations->storeGeometryObject(batchVertex, batchIndex, transformBuffer,
                                                      indirectMapPoint[i].vertexOffset * batchVertex->getStepSize(),
                                                      indirectMapPoint[i].firstIndex * sizeof(uint32_t),
                                                      i * sizeof(VkTransformMatrixKHR),
                                                      indirectMapPoint[i].indexCount, vertexEnd);
        geometryNodes.push_back({i, vertexAddress+indirectMapPoint[i].vertexOffset * batchVertex->getStepSize(),(uint32_t)batchVertex->getStepSize(),
                                 indexAddress+indirectMapPoint[i].firstIndex * sizeof(uint32_t)});
    }
    geometryNodesStorage = std::make_shared<StorageBuffer>(device, geometryNodes.size() * sizeof(GeometryNode),
                                                           rtSupport ? vk::BufferUsageFlagBits::eShaderDeviceAddress
                                                                     : vk::BufferUsageFlags());
    memcpy(geometryNodesStorage->getMapPoint(), geometryNodes.data(), geometryNodes.size() * sizeof(GeometryNode));

    bottomLevelAccelerations->confirmObjectsAndCreateBLASes();
    topLevelAccelerationStructure = std::make_shared<vkLibRt::TopLevelAccelerationStructure>(instance, device);
    topLevelAccelerationStructure->createTlas(bottomLevelAccelerations->getAccelerationStructures());

}

uint32_t RenderObjectsDataManager::pushMaterial(Material &material) {
    materialsBuffer.push_back(material);
    return materialsBuffer.size()-1;
}

std::shared_ptr<IndexBuffer> RenderObjectsDataManager::getBatchIndex() const {
    return batchIndex;
}

std::shared_ptr<VertexBuffer> RenderObjectsDataManager::getBatchVertex() const {
    return batchVertex;
}

std::shared_ptr<IndirectBuffer> RenderObjectsDataManager::getIndirectBuffer() const {
    return indirectBuffer;
}

std::shared_ptr<Buffer> RenderObjectsDataManager::getTransformBuffer() const {
    return transformBuffer;
}

std::shared_ptr<vkLibRt::TopLevelAccelerationStructure>
RenderObjectsDataManager::getTopLevelAccelerationStructure() const {
    return topLevelAccelerationStructure;
}

uint32_t RenderObjectsDataManager::pushImage(std::shared_ptr<Image> image) {
    auto sampler = std::make_shared<Sampler>(device);
    textureBuffer.push_back(std::pair(sampler, image));
    return textureBuffer.size()-1;
}

void
RenderObjectsDataManager::writeInfoAboutAcceleration(std::shared_ptr<DescriptorSet> descriptorSet) {
    if (topLevelAccelerationStructure) {
        DescriptorAccelerationStructureInfo info{{topLevelAccelerationStructure->getAccelerationStructure()},
                                                 RT_AS_INFO_BINDING, vk::DescriptorType::eAccelerationStructureKHR};
        descriptorSet->addAccelerationStructureInfo(info);
        DescriptorBufferInfo geometryInfo{
                {{geometryNodesStorage->getBuffer(), 0, geometryNodesStorage->getBufferSize()}},
                RT_GEOMETRY_NODE_BINDING, vk::DescriptorType::eStorageBuffer};
        descriptorSet->addBufferInfo(geometryInfo);
    }
}

void RenderObjectsDataManager::writeMaterialsInfos(std::shared_ptr<DescriptorSet> descriptorSet) {
    DescriptorBufferInfo bufferInfo{{{materialsStorage->getBuffer(), 0, materialsStorage->getBufferSize()}},
                                    MATERIALS_BUFFER_BINDING, vk::DescriptorType::eStorageBuffer};
    descriptorSet->addBufferInfo(bufferInfo);

}

void RenderObjectsDataManager::writeImagesInfos(std::shared_ptr<DescriptorSet> descriptorSet) {
    DescriptorImageInfo imageInfo{};
    for (const auto &item: textureBuffer) {
        imageInfo.imageInfos.push_back({item.first->getSampler(), item.second->getImageViews()[0]->getBase(), item.second->getImageInfo().initialLayout});
    }
    imageInfo.binding = TEXTURES_BUFFER_BINDING;
    imageInfo.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    descriptorSet->addImageInfo(imageInfo);
}

const std::shared_ptr<StorageBuffer> &RenderObjectsDataManager::getGeometryNodesStorage() const {
    return geometryNodesStorage;
}

void RenderObjectsDataManager::presetBuilder(std::shared_ptr<RenderPipelineBuilder> builder, vk::ShaderStageFlags flags) {
    builder->addSamplerInfo({0, static_cast<uint32_t>(textureBuffer.size()), flags});
    builder->addStorageBufferInfo({1, 1, flags});

}

void RenderObjectsDataManager::presetBuilder(std::shared_ptr<vkLibRt::RayTracingPipelineBuilder> builder, vk::ShaderStageFlags flags) {
    builder->addSamplerInfo({0, static_cast<uint32_t>(textureBuffer.size()), flags});
    builder->addStorageBufferInfo({1, 1, flags});
    builder->addAccelerationStructureInfo({2, 1, flags});
    builder->addStorageImageInfo({3, 1, flags});
    builder->addStorageBufferInfo({4, 1, flags});
}
#include "PBRPipeline.h"
#include "Pipelines/Standart/GBufferPipeline.hpp"
#include "RenderData/DescriptorSetData.hpp"
#include "RenderData/Quad.hpp"
#include "VulkanLib/Device/Buffer/StorageBuffer.hpp"
#include "VulkanLib/Instance.hpp"
#include "VulkanLib/Shader/ShaderCreateInfo.hpp"
#include "vulkan/vulkan_enums.hpp"
#include <memory>

PBRPipeline::PBRPipeline(const std::shared_ptr<LogicalDevice> &device, std::shared_ptr<GBufferPipeline> gbPipeline, uint32_t maxPointLighsAmount, uint32_t maxDirectLightBlocks) : 
device(device), maxPointLighsAmount(maxPointLighsAmount), maxDirectLightBlocks(maxDirectLightBlocks), gbPipeline(gbPipeline){
    pointLightsStorage = std::make_shared<StorageBuffer>(device, maxPointLighsAmount*sizeof(PointLight), 0);
    directLightsStorage = std::make_shared<StorageBuffer>(device, maxDirectLightBlocks*sizeof(DirectLight), 0);
    quadMesh = QuadMesh::getQuadMesh(device);
}

void PBRPipeline::populateBuilder(std::shared_ptr<RenderPipelineBuilder> builder)
{
    builder->addVertexInput({0, 3, sizeof(float), vk::Format::eR32G32B32Sfloat});
    builder->addVertexInput({0, 2, sizeof(float), vk::Format::eR32G32Sfloat});
    builder->addUniformBuffer({6, sizeof(LighConfig), 1, vk::ShaderStageFlagBits::eFragment});
    builder->addStorageBufferInfo({5, 1, vk::ShaderStageFlagBits::eFragment});
    builder->addStorageBufferInfo({4, 1, vk::ShaderStageFlagBits::eFragment});
    for(uint8_t i = 0; i<4; i++)
        builder->addSamplerInfo({i ,1, vk::ShaderStageFlagBits::eFragment});
}

std::shared_ptr<std::vector<ShaderCreateInfo> > PBRPipeline::getShadersInfos()
{
    auto res = std::make_shared<std::vector<ShaderCreateInfo>>(2);
    (*res)[0] = { "shaders/PBRPipeline/main.vert", "main.vert", SRC_FILE, vk::ShaderStageFlagBits::eVertex };
    (*res)[1] = { "shaders/PBRPipeline/main.frag", "main.frag", SRC_FILE, vk::ShaderStageFlagBits::eFragment };
    return res;
}


void PBRPipeline::render(vk::CommandBuffer cmd, uint32_t currentImage)
{
     quadMesh.second->bind(cmd);
    quadMesh.first->bind(cmd);
    quadMesh.first->drawAll(cmd);
}

void PBRPipeline::render(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<Mesh> mesh, std::shared_ptr<IndirectBuffer> indirectBuffer, size_t indirectOffset)
{

}

void PBRPipeline::setDescriptorPool(std::shared_ptr<DescriptorPool> descriptorPool)
{
    PBRPipeline::descriptorPool = descriptorPool;
    descriptorSetData = std::make_shared<DescriptorSetData>(descriptorPool->allocateDescriptorSet(framesInFlight,renderPipeline->getGraphicsPipeline()->getDescriptorLayout()));
    for(uint8_t i = 0; i<4; i++)
        descriptorSetData->createSampler(device, i);
    descriptorSetData->createUniformBuffer(device, 6, sizeof(LighConfig));
    for(uint8_t i = 0; i<framesInFlight; i++){
        descriptorSetData->setStorageBuffer(pointLightsStorage, 5, i);
        descriptorSetData->setStorageBuffer(directLightsStorage, 4, i);
    }


}

void PBRPipeline::setRenderPipeline(std::shared_ptr<GraphicsRenderPipeline> renderPipeline)
{
    PBRPipeline::renderPipeline = renderPipeline;
}

bool PBRPipeline::attachToSwapChain()
{
    return false;
}

void PBRPipeline::setMaxFramesInFlight(uint32_t framesInFlight)
{
    PBRPipeline::framesInFlight = framesInFlight;
}

void PBRPipeline::setup()
{
    for(uint8_t j = 0; j<framesInFlight; j++){
        for(uint8_t i = 0; i<4; i++){
            auto imageView = gbPipeline->getRenderPipeline()->getBaseRenderImages()[j*i]->getImageViews()[0];
            descriptorSetData->getSamplerByBinding(i, j)->imageView = imageView->getBase();
        }
    }
    descriptorSetData->confirmAndWriteDataToDescriptorSet();
}

void PBRPipeline::resized(uint32_t width, uint32_t height)
{
    device->getDevice().waitIdle();
    setup();
    device->getDevice().waitIdle();
}

void PBRPipeline::beginRender(vk::CommandBuffer cmd, uint32_t currentImage)
{
    renderPipeline->begin(cmd, currentImage);
	descriptorSetData->getDescriptorSet()->bindDescriptor(vk::PipelineBindPoint::eGraphics, currentImage, cmd, renderPipeline->getGraphicsPipeline()->getPipelineLayout());
}


void PBRPipeline::endRender(vk::CommandBuffer cmd, uint32_t currentImage)
{
    renderPipeline->endRender(cmd, currentImage);
}

void PBRPipeline::bindBatchData(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<RenderObjectsDataManager> batchData)
{

}

void PBRPipeline::setupGlobalDescriptor(std::shared_ptr<RenderObjectsDataManager> dataManager)
{

}

std::shared_ptr<GraphicsRenderPipeline> PBRPipeline::getRenderPipeline() const
{
    return renderPipeline;
}

void PBRPipeline::setCameraPosition(glm::vec3 camPos)
{
    for(uint8_t i = 0; i<framesInFlight; i++)
        ((LighConfig*)descriptorSetData->getUniformBufferByBinding(6, i)->getMapPoint())->cameraPosition = camPos;
}

void PBRPipeline::setupMesh(std::shared_ptr<Mesh> mesh)
{

}

bool PBRPipeline::processMeshes()
{
    return false;
}

void PBRPipeline::destroy()
{
    renderPipeline->destroy();
}

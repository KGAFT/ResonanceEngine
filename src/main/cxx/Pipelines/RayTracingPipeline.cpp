//
// Created by kgaft on 8/30/24.
//

#include "RayTracingPipeline.hpp"

#include "VulkanLib/Device/Descriptors/DescriptorPool.hpp"

vk::ImageCreateInfo defaultColorCreateInfo = {vk::ImageCreateFlags(),
                                              vk::ImageType::e2D, vk::Format::eR32G32B32A32Sfloat,
                                              vk::Extent3D{800, 600, 1},
                                              1, 1, vk::SampleCountFlagBits::e1,
                                              vk::ImageTiling::eOptimal,
                                              vk::ImageUsageFlagBits::eColorAttachment |
                                              vk::ImageUsageFlagBits::eSampled |
                                              vk::ImageUsageFlagBits::eTransferSrc |
                                              vk::ImageUsageFlagBits::eStorage,
                                              vk::SharingMode::eExclusive,
                                              0, nullptr,
                                              vk::ImageLayout::eUndefined, nullptr

};


vk::ImageViewCreateInfo defaultColorViewCreateInfo = {vk::ImageViewCreateFlags(),
                                                      vk::Image(), vk::ImageViewType::e2D,
                                                      vk::Format::eR32G32B32A32Sfloat, {},
                                                      vk::ImageSubresourceRange{
                                                              vk::ImageAspectFlagBits::eColor, 0, 1,
                                                              0, 1}};

RayTracingPipeline::RayTracingPipeline(std::shared_ptr<LogicalDevice> &device, Instance &instance,
                                       std::shared_ptr<vkLibRt::RayTracingPipelineBuilder>& rtBuilder,
                                       std::vector<ShaderCreateInfo> &shadersInfos) :
        device(device), instance(instance), builder(rtBuilder), createInfos(shadersInfos) {

}

void RayTracingPipeline::setup(std::shared_ptr<DescriptorPool> descriptorPool, uint32_t width, uint32_t height) {
    renderPipeline = std::make_shared<vkLibRt::RayTracingRenderPipeline>(instance, device, builder, shader);
    pipelineDescriptor = descriptorPool->allocateDescriptorSet(1, renderPipeline->getRayTracingPipeline()->getDescriptorSetLayout());
    defaultColorCreateInfo.extent.width = width;
    defaultColorCreateInfo.extent.height = height;
    renderOutput = std::make_shared<Image>(device, defaultColorCreateInfo);
    renderOutput->transitionImageLayout(device, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, vk::ImageAspectFlagBits::eColor);
    renderOutput->getImageInfo().initialLayout = vk::ImageLayout::eGeneral;
    defaultColorViewCreateInfo.image = renderOutput->getBase();
    renderOutput->createImageView(defaultColorViewCreateInfo);
}

void RayTracingPipeline::setShader(Shader *newShader) {
    shader = newShader;
}

std::vector<ShaderCreateInfo> RayTracingPipeline::getCreateInfos() const {
    return createInfos;
}

void RayTracingPipeline::processRenderObjects(std::shared_ptr<RenderObjectsDataManager> dataManager) {
    RayTracingPipeline::dataManager = dataManager;
    renderPipeline = std::make_shared<vkLibRt::RayTracingRenderPipeline>(instance, device, builder, shader);
    dataManager->writeImagesInfos(pipelineDescriptor);
    dataManager->writeInfoAboutAcceleration(pipelineDescriptor);
    dataManager->writeMaterialsInfos(pipelineDescriptor);
    DescriptorImageInfo imageInfo = {{{{}, renderOutput->getImageViews()[0]->getBase(), vk::ImageLayout::eGeneral}}, RT_OUTPUT_IMAGE_BINDING,
                                     vk::DescriptorType::eStorageImage};
    pipelineDescriptor->addImageInfo(imageInfo);
    pipelineDescriptor->updateDescriptors();
    pipelineDescriptor->clearObjectsInfos();
}

void RayTracingPipeline::beginRayTrace(vk::CommandBuffer cmd) {
    renderPipeline->beginRayTrace(cmd);
    pipelineDescriptor->bindDescriptor(vk::PipelineBindPoint::eRayTracingKHR, 0, cmd, renderPipeline->getRayTracingPipeline()->getPipelineLayout());

}

void RayTracingPipeline::rayTrace(vk::CommandBuffer cmd, uint32_t width, uint32_t height) {
    renderPipeline->rayTrace(cmd, width, height);
}

const std::shared_ptr<DescriptorSet> &RayTracingPipeline::getPipelineDescriptor() const {
    return pipelineDescriptor;
}

void RayTracingPipeline::resize(uint32_t width, uint32_t height) {
    renderOutput->resize(width, height);
    processRenderObjects(dataManager);
}

const std::shared_ptr<Image> &RayTracingPipeline::getRenderOutput() const {
    return renderOutput;
}

const std::shared_ptr<vkLibRt::RayTracingRenderPipeline> &RayTracingPipeline::getRenderPipeline() const {
    return renderPipeline;
}
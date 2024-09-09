//
// Created by kgaft on 8/31/24.
//

#include "GBufferPipeline.hpp"

void GBufferPipeline::populateBuilder(std::shared_ptr<RenderPipelineBuilder> builder) {
	dataManager->presetBuilder(builder, vk::ShaderStageFlagBits::eFragment);
    builder->addPushConstantInfo({vk::ShaderStageFlagBits::eVertex|vk::ShaderStageFlagBits::eFragment, sizeof(WorldTransformData)});
	builder->addVertexInput({ 0, 3, sizeof(float), vk::Format::eR32G32B32Sfloat });
	builder->addVertexInput({ 1, 3, sizeof(float), vk::Format::eR32G32B32Sfloat });
	builder->addVertexInput({ 2, 2, sizeof(float), vk::Format::eR32G32Sfloat });
	builder->addVertexInput({ 3, 1, sizeof(float), vk::Format::eR32Sfloat });
	builder->setAttachmentsPerStepAmount(4);
}

std::shared_ptr<std::vector<ShaderCreateInfo>> GBufferPipeline::getShadersInfos() {
	auto res = std::make_shared<std::vector<ShaderCreateInfo>>();
	res->push_back({ "shaders/GBufferPipeline/main.vert", "main.vert", SRC_FILE, vk::ShaderStageFlagBits::eVertex });
	res->push_back({ "shaders/GBufferPipeline/main.frag", "main.frag", SRC_FILE, vk::ShaderStageFlagBits::eFragment });
	return res;
}

void GBufferPipeline::render(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<Mesh> mesh,
	std::shared_ptr<IndirectBuffer> indirectBuffer, size_t indirectOffset) {
	data.worldMatrix = mesh->calculateWorldMatrix();
    pushConstant->writeData(&data, sizeof(WorldTransformData));
    pushConstant->bind(cmd, vk::ShaderStageFlagBits::eVertex|vk::ShaderStageFlagBits::eFragment);
	cmd.drawIndexedIndirect(indirectBuffer->getBuffer(), mesh->getIndirectOffset(), 1, (uint32_t)indirectBuffer->getSizeOfStructure());
}

void GBufferPipeline::beginRender(vk::CommandBuffer cmd, uint32_t currentImage) {
	renderPipeline->begin(cmd, currentImage);
	renderData->bindDescriptor(vk::PipelineBindPoint::eGraphics, currentImage, cmd, renderPipeline->getGraphicsPipeline()->getPipelineLayout());
}

void GBufferPipeline::setDescriptorPool(std::shared_ptr<DescriptorPool> descriptorPool) {
	GBufferPipeline::descriptorPool = descriptorPool;
}

void GBufferPipeline::setRenderPipeline(std::shared_ptr<GraphicsRenderPipeline> renderPipeline) {
	GBufferPipeline::renderPipeline = renderPipeline;
}

bool GBufferPipeline::attachToSwapChain() {
	return false;
}

void GBufferPipeline::setMaxFramesInFlight(uint32_t framesInFlight) {
	GBufferPipeline::framesInFlight = framesInFlight;
}

void GBufferPipeline::setup() {

}

void GBufferPipeline::resized(uint32_t width, uint32_t height) {
	device->getDevice().waitIdle();
	renderPipeline->resize(width, height);
}

void GBufferPipeline::render(vk::CommandBuffer cmd, uint32_t currentImage) {

}

void GBufferPipeline::endRender(vk::CommandBuffer cmd, uint32_t currentImage) {
	renderPipeline->endRender(cmd, currentImage);
}

void GBufferPipeline::bindBatchData(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<RenderObjectsDataManager> batchData) {
	batchData->getBatchVertex()->bind(cmd);
	batchData->getBatchIndex()->bind(cmd);
}

void GBufferPipeline::setupGlobalDescriptor(std::shared_ptr<RenderObjectsDataManager> dataManager) {
	renderData = descriptorPool->allocateDescriptorSet(framesInFlight, renderPipeline->getGraphicsPipeline()->getDescriptorLayout());
    pushConstant = std::make_shared<PushConstant>(sizeof(WorldTransformData), renderPipeline->getGraphicsPipeline()->getPipelineLayout());
	dataManager->writeImagesInfos(renderData);
	dataManager->writeMaterialsInfos(renderData);
	renderData->updateDescriptors();
	renderData->clearObjectsInfos();
}

std::shared_ptr<GraphicsRenderPipeline> GBufferPipeline::getRenderPipeline() const {
	return renderPipeline;
}

void GBufferPipeline::setCameraMatrix(glm::mat4 cameraMatrix, glm::vec3 camPos) {
    data.viewMatrix = cameraMatrix;
}


void GBufferPipeline::setupMesh(std::shared_ptr<Mesh> mesh) {

}

bool GBufferPipeline::processMeshes() {
	return true;
}


void GBufferPipeline::destroy() {
    pushConstant->destroy();
	renderPipeline->destroy();
}

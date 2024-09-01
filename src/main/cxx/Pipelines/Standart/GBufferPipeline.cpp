//
// Created by kgaft on 8/31/24.
//

#include "GBufferPipeline.hpp"

void GBufferPipeline::populateBuilder(std::shared_ptr<RenderPipelineBuilder> builder) {
	renderObjectsManager->presetBuilder(builder, vk::ShaderStageFlagBits::eFragment);
	builder->addUniformBuffer({ 3, sizeof(WorldTransformData), 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment });
	builder->addVertexInput({ 0, 3, sizeof(float), vk::Format::eR32G32B32Sfloat });
	builder->addVertexInput({ 1, 3, sizeof(float), vk::Format::eR32G32B32Sfloat });
	builder->addVertexInput({ 2, 2, sizeof(float), vk::Format::eR32G32Sfloat });
	builder->addVertexInput({ 3, 1, sizeof(int32_t), vk::Format::eR32Sint });
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
	memcpy(renderData->getUniformBufferByBinding(3)->getMapPoint(), &data, sizeof(WorldTransformData));
	cmd.drawIndexedIndirect(indirectBuffer->getBuffer(), mesh->getIndirectOffset(), 1, (uint32_t)indirectBuffer->getSizeOfStructure());
}

void GBufferPipeline::beginRender(vk::CommandBuffer cmd, uint32_t currentImage) {
	renderPipeline->begin(cmd, currentImage);
	renderData->getDescriptorSet()->bindDescriptor(vk::PipelineBindPoint::eGraphics, currentImage, cmd, renderPipeline->getGraphicsPipeline()->getPipelineLayout());
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
	renderObjectsManager->
}
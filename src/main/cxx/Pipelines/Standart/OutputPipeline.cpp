//
// Created by kgaft on 3/12/24.
//

#include "OutputPipeline.hpp"

void OutputPipeline::populateBuilder(std::shared_ptr<RenderPipelineBuilder> builder) {
    builder->addVertexInput({ 0, 3, sizeof(float), vk::Format::eR32G32B32Sfloat });
    builder->addVertexInput({ 1, 2, sizeof(float), vk::Format::eR32G32Sfloat });
    builder->addSamplerInfo({ 0, 1, vk::ShaderStageFlagBits::eFragment });
    builder->addSamplerInfo({ 1, 1, vk::ShaderStageFlagBits::eFragment });
    builder->addSamplerInfo({ 2, 1, vk::ShaderStageFlagBits::eFragment });
    builder->addPushConstantInfo({ vk::ShaderStageFlagBits::eFragment, sizeof(OutputPipelineConfig) });
    builder->setAttachmentsPerStepAmount(1);
}

std::shared_ptr<std::vector<ShaderCreateInfo>> OutputPipeline::getShadersInfos()  {
    std::shared_ptr<std::vector<ShaderCreateInfo>> res = std::make_shared<std::vector<ShaderCreateInfo>>();
    res->push_back({ "shaders/OutputPipeline/main.vert", "main.vert", SRC_FILE, vk::ShaderStageFlagBits::eVertex });
    res->push_back({ "shaders/OutputPipeline/main.frag", "main.frag", SRC_FILE, vk::ShaderStageFlagBits::eFragment });
    return res;
}

void OutputPipeline::render(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<Mesh> mesh,
    std::shared_ptr<IndirectBuffer> indirectBuffer, size_t indirectOffset) {
}

void OutputPipeline::setDescriptorPool(std::shared_ptr<DescriptorPool> descriptorPool) {
    OutputPipeline::descriptorPool = descriptorPool;
}

void OutputPipeline::setRenderPipeline(std::shared_ptr<GraphicsRenderPipeline> renderPipeline) {
    OutputPipeline::renderPipeline = renderPipeline;
}

bool OutputPipeline::attachToSwapChain() {
    return true;
}

void OutputPipeline::setMaxFramesInFlight(uint32_t framesInFlight)  {
    OutputPipeline::framesInFlight = framesInFlight;
}

void OutputPipeline::setup() {
    quadMesh = QuadMesh::getQuadMesh(device);
    auto descriptorSet = descriptorPool->allocateDescriptorSet(framesInFlight,
        renderPipeline->getGraphicsPipeline()->getDescriptorLayout());
    renderData = std::make_shared<DescriptorSetData>(descriptorSet);
    pushConstant = std::make_shared<PushConstant>(sizeof(OutputPipelineConfig),
        renderPipeline->getGraphicsPipeline()->getPipelineLayout());
    updateDescriptors(false);
}

void OutputPipeline::resized(uint32_t width, uint32_t height) {
    device->getDevice().waitIdle();
    renderPipeline->resize(width, height);
    updateDescriptors(true);
}

void OutputPipeline::beginRender(vk::CommandBuffer cmd, uint32_t currentImage)  {
    renderPipeline->begin(cmd, currentImage);
}

void OutputPipeline::render(vk::CommandBuffer cmd, uint32_t currentImage)  {
    renderData->getDescriptorSet()->bindDescriptor(vk::PipelineBindPoint::eGraphics, currentImage, cmd,
        renderPipeline->getGraphicsPipeline()->getPipelineLayout());
    pushConstant->writeData(&config, sizeof(OutputPipelineConfig));
    pushConstant->bind(cmd, vk::ShaderStageFlagBits::eFragment);
    quadMesh.second->bind(cmd);
    quadMesh.first->bind(cmd);
    quadMesh.first->drawAll(cmd);
}
void OutputPipeline::addAttachment(std::shared_ptr<ImageView> attachment) {
    if ((!attachments.empty()) && attachments.size() / framesInFlight == 3)
        throw std::runtime_error("Error: you cannot add more attachments");
    attachments.push_back(attachment);
}

void OutputPipeline::updateDescriptors(bool afterResize) {
    if (!afterResize) {
        renderData->setInstanceCount(framesInFlight);
        for (uint32_t inst = 0; inst < framesInFlight; inst++) {
            for (uint32_t i = 0; i < 3; i++) {
                renderData->createSampler(device, i, inst);
            }
        }
    }
    for (uint32_t i = 0; i < attachments.size(); ++i) {
        uint32_t bind = i / framesInFlight;
        uint32_t inst = i - (bind * framesInFlight);
        renderData->getSamplerByBinding(bind, inst)->imageView = attachments[i]->getBase();
        renderData->getSamplerByBinding(bind, inst)->layout = attachments[i]->getParentInfo().initialLayout;
    }

    renderData->confirmAndWriteDataToDescriptorSet();
}

void OutputPipeline::endRender(vk::CommandBuffer cmd, uint32_t currentImage)  {
    renderPipeline->endRender(cmd, currentImage);
}

void OutputPipeline::bindBatchData(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<RenderObjectsDataManager> batchData)  {

}
void OutputPipeline::setupGlobalDescriptor(std::shared_ptr<RenderObjectsDataManager> dataManager)  {

}



void OutputPipeline::setupMesh(std::shared_ptr<Mesh> mesh)  {

}


bool OutputPipeline::processMeshes()  {
    return false;
}


void OutputPipeline::destroy()  {
    destroyed = true;
    renderData->destroy();
    pushConstant->destroy();
    renderPipeline->destroy();
}


    void OutputPipeline::keyPressed(WindowKey* key)  {
        switch (key->scanCode) {
        case WINDOW_KEY(F2):
            if (config.enableInput[0])
                config.enableInput[0] = 0;
            else
                config.enableInput[0] = 1;
            break;
        case WINDOW_KEY(F3):
            if (config.enableInput[1])
                config.enableInput[1] = 0;
            else
                config.enableInput[1] = 1;
            break;
        case WINDOW_KEY(F4):
            if (config.enableInput[2])
                config.enableInput[2] = 0;
            else
                config.enableInput[2] = 1;
            break;
        }
    }

    unsigned int OutputPipeline::getKeys(WindowKey** output)  {
        if (windowKeys.empty()) {
            windowKeys.resize(3);
            windowKeys[0] = { WINDOW_KEY(F2), KEY_CLICKED, false };
            windowKeys[1] = { WINDOW_KEY(F3), KEY_CLICKED, false };
            windowKeys[2] = { WINDOW_KEY(F4), KEY_CLICKED, false };

        }
        *output = windowKeys.data();
        return windowKeys.size();
    }
size_t OutputPipeline::getHoldDelay()  {
    return 0;
}

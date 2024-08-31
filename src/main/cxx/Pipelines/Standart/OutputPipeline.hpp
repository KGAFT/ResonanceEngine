//
// Created by kgaft on 3/12/24.
//
#ifndef OUTPUTPIPELINE_HPP
#define OUTPUTPIPELINE_HPP

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include "IPipelineGraphics.hxx"
#include "VulkanLib/Device/Buffer/PushConstant.hpp"
#include "RenderData/DescriptorSetData.hpp"
#include "RenderData/Mesh.hpp"
#include "RenderData/Quad.hpp"
#include "VulkanLib/Device/Buffer/IndexBuffer.hpp"
#include "VulkanLib/Device/Buffer/IndirectBuffer.hpp"
#include "VulkanLib/Device/Buffer/VertexBuffer.hpp"
#include "Window/Window.hxx"

struct OutputPipelineConfig {
    int enableInput[3] = {0,0,0};
    int dep;
};

class OutputPipeline : public IPipelineGraphics, public IWindowKeyCallback {
public:
    explicit OutputPipeline(const std::shared_ptr<LogicalDevice> &device) : device(device) {}

private:
    std::shared_ptr<LogicalDevice> device;
    std::shared_ptr<DescriptorPool> descriptorPool;
    std::shared_ptr<GraphicsRenderPipeline> renderPipeline;
    uint32_t pipelineId;
    uint32_t framesInFlight;
    OutputPipelineConfig config{};
    std::shared_ptr<PushConstant> pushConstant;
    std::pair<std::shared_ptr<IndexBuffer>, std::shared_ptr<VertexBuffer>> quadMesh;
    std::shared_ptr<DescriptorSetData> renderData;
    std::vector<std::shared_ptr<ImageView>> attachments;
    std::vector<WindowKey> windowKeys;
public:
    void populateBuilder(std::shared_ptr<RenderPipelineBuilder> builder) override {
        builder->addVertexInput({0, 3, sizeof(float), vk::Format::eR32G32B32Sfloat});
        builder->addVertexInput({1, 2, sizeof(float), vk::Format::eR32G32Sfloat});
        builder->addSamplerInfo({0, 1, vk::ShaderStageFlagBits::eFragment});
        builder->addSamplerInfo({1, 1, vk::ShaderStageFlagBits::eFragment});
        builder->addSamplerInfo({2, 1, vk::ShaderStageFlagBits::eFragment});
        builder->addPushConstantInfo({vk::ShaderStageFlagBits::eFragment, sizeof(OutputPipelineConfig)});
        builder->setAttachmentsPerStepAmount(1);
    }

    std::shared_ptr<std::vector<ShaderCreateInfo>> getShadersInfos() override {
        std::shared_ptr<std::vector<ShaderCreateInfo>> res = std::make_shared<std::vector<ShaderCreateInfo>>();
        res->push_back({"shaders/OutputPipeline/main.vert", "main.vert", SRC_FILE, vk::ShaderStageFlagBits::eVertex});
        res->push_back({"shaders/OutputPipeline/main.frag", "main.frag", SRC_FILE, vk::ShaderStageFlagBits::eFragment});
        return res;
    }

    void render(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<Mesh> mesh,
                std::shared_ptr<IndirectBuffer> indirectBuffer, size_t indirectOffset) override {

    }

    void setDescriptorPool(std::shared_ptr<DescriptorPool> descriptorPool) override {
        OutputPipeline::descriptorPool = descriptorPool;
    }


    void setRenderPipeline(std::shared_ptr<GraphicsRenderPipeline> renderPipeline) override {
        OutputPipeline::renderPipeline = renderPipeline;
    }

    bool attachToSwapChain() override {
        return true;
    }

    void setMaxFramesInFlight(uint32_t framesInFlight) override {
        OutputPipeline::framesInFlight = framesInFlight;
    }

    void setup() override {
        quadMesh = QuadMesh::getQuadMesh(device);
        auto descriptorSet = descriptorPool->allocateDescriptorSet(framesInFlight,
                                                                   renderPipeline->getGraphicsPipeline()->getDescriptorLayout());
        renderData = std::make_shared<DescriptorSetData>(descriptorSet);
        pushConstant = std::make_shared<PushConstant>(sizeof(OutputPipelineConfig),
                                                      renderPipeline->getGraphicsPipeline()->getPipelineLayout());
        updateDescriptors(false);
    }

    void resized(uint32_t width, uint32_t height) override {
        device->getDevice().waitIdle();
        renderPipeline->resize(width, height);
        updateDescriptors(true);
    }

    void beginRender(vk::CommandBuffer cmd, uint32_t currentImage) override {
        renderPipeline->begin(cmd, currentImage);
    }

    void render(vk::CommandBuffer cmd, uint32_t currentImage) override {
        renderData->getDescriptorSet()->bindDescriptor(vk::PipelineBindPoint::eGraphics, currentImage, cmd,
                                                       renderPipeline->getGraphicsPipeline()->getPipelineLayout());
        pushConstant->writeData(&config, sizeof(OutputPipelineConfig));
        pushConstant->bind(cmd, vk::ShaderStageFlagBits::eFragment);
        quadMesh.second->bind(cmd);
        quadMesh.first->bind(cmd);
        quadMesh.first->drawAll(cmd);
    }
    void addAttachment(std::shared_ptr<ImageView> attachment){
        if((!attachments.empty())&&attachments.size()/framesInFlight==3)
            throw std::runtime_error("Error: you cannot add more attachments");
        attachments.push_back(attachment);
    }

    void updateDescriptors(bool afterResize){
        if(!afterResize){
            renderData->setInstanceCount(framesInFlight);
            for (uint32_t inst = 0; inst < framesInFlight; inst++) {
                for (uint32_t i = 0; i < 3; i++) {
                    renderData->createSampler(device, i, inst);
                }
            }
        }
        for (uint32_t i = 0; i < attachments.size(); ++i){
            uint32_t bind = i/framesInFlight;
            uint32_t inst = i-(bind*framesInFlight);
            renderData->getSamplerByBinding(bind, inst)->imageView = attachments[i]->getBase();
            renderData->getSamplerByBinding(bind, inst)->layout = attachments[i]->getParentInfo().initialLayout;
        }

        renderData->confirmAndWriteDataToDescriptorSet();
    }

    void endRender(vk::CommandBuffer cmd, uint32_t currentImage) override {
        renderPipeline->endRender(cmd, currentImage);
    }

    void bindBatchData(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<RenderObjectsDataManager> batchData) override{

    }
    void setupGlobalDescriptor(std::shared_ptr<RenderObjectsDataManager> dataManager) override{

    }



    void setupMesh(std::shared_ptr<Mesh> mesh) override {

    }


    bool processMeshes() override {
        return false;
    }


    void destroy() override {
        destroyed = true;
        renderData->destroy();
        pushConstant->destroy();
        renderPipeline->destroy();
    }

private:
    void keyPressed(WindowKey *key) override {
        switch (key->scanCode) {
            case WINDOW_KEY(F2):
                if(config.enableInput[0])
                    config.enableInput[0] = 0;
                else
                    config.enableInput[0] = 1;
                break;
            case WINDOW_KEY(F3):
                if(config.enableInput[1])
                    config.enableInput[1] = 0;
                else
                    config.enableInput[1] = 1;
                break;
            case WINDOW_KEY(F4):
                if(config.enableInput[2])
                    config.enableInput[2] = 0;
                else
                    config.enableInput[2] = 1;
                break;
        }
    }

    unsigned int getKeys(WindowKey **output) override {
        if(windowKeys.empty()){
            windowKeys.resize(3);
            windowKeys[0] = {WINDOW_KEY(F2), KEY_CLICKED, false};
            windowKeys[1] = {WINDOW_KEY(F3), KEY_CLICKED, false};
            windowKeys[2] = {WINDOW_KEY(F4), KEY_CLICKED, false};

        }
        *output = windowKeys.data();
        return windowKeys.size();
    }

    size_t getHoldDelay() override {
        return 0;
    }
};


#endif //VULKANRENDERENGINE_OUTPUTPIPELINE_HPP
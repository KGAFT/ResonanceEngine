//
// Created by kgaft on 3/12/24.
//
#ifndef OUTPUTPIPELINE_HPP
#define OUTPUTPIPELINE_HPP

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include "../IPipelineGraphics.hxx"
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
    void populateBuilder(std::shared_ptr<RenderPipelineBuilder> builder) override;

    std::shared_ptr<std::vector<ShaderCreateInfo>> getShadersInfos() override;
    

    void render(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<Mesh> mesh,
        std::shared_ptr<IndirectBuffer> indirectBuffer, size_t indirectOffset) override;

    void setDescriptorPool(std::shared_ptr<DescriptorPool> descriptorPool) override;

    void setRenderPipeline(std::shared_ptr<GraphicsRenderPipeline> renderPipeline) override;

    bool attachToSwapChain() override;

    void setMaxFramesInFlight(uint32_t framesInFlight) override;

    void setup() override;

    void resized(uint32_t width, uint32_t height) override;

    void beginRender(vk::CommandBuffer cmd, uint32_t currentImage) override;

    void render(vk::CommandBuffer cmd, uint32_t currentImage) override;
    void addAttachment(std::shared_ptr<ImageView> attachment);

    void updateDescriptors(bool afterResize);

    void endRender(vk::CommandBuffer cmd, uint32_t currentImage) override;

    void bindBatchData(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<RenderObjectsDataManager> batchData) override;
    void setupGlobalDescriptor(std::shared_ptr<RenderObjectsDataManager> dataManager) override;



    void setupMesh(std::shared_ptr<Mesh> mesh) override;

    bool processMeshes() override;


    void destroy() override;

private:
    void keyPressed(WindowKey* key) override;

    unsigned int getKeys(WindowKey** output) override;

    size_t getHoldDelay() override;
};


#endif //VULKANRENDERENGINE_OUTPUTPIPELINE_HPP
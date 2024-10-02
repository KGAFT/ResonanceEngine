//
// Created by kgaft on 8/31/24.
//

#ifndef GBUFFERPIPELINE_HPP
#define GBUFFERPIPELINE_HPP


#include "Pipelines/IPipelineGraphics.hxx"
#include <VulkanLib/Device/Buffer/PushConstant.hpp>
#include <RenderData/DescriptorSetData.hpp>

struct WorldTransformData {
    glm::mat4 viewMatrix;
    glm::mat4 worldMatrix;
};



class GBufferPipeline : public IPipelineGraphics{
public:
    GBufferPipeline(const std::shared_ptr<LogicalDevice>& device, std::shared_ptr<RenderObjectsDataManager> dataManager) : device(device), dataManager(dataManager){}
private:
    std::shared_ptr<LogicalDevice> device;
    std::shared_ptr<DescriptorPool> descriptorPool;
    std::shared_ptr<GraphicsRenderPipeline> renderPipeline;
    uint32_t pipelineId;
    uint32_t framesInFlight;
    std::shared_ptr<PushConstant> pushConstant;
    std::pair<std::shared_ptr<IndexBuffer>, std::shared_ptr<VertexBuffer>> quadMesh;
    std::shared_ptr<DescriptorSet> renderData;
    std::shared_ptr<RenderObjectsDataManager> dataManager;
    WorldTransformData data;
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

    void endRender(vk::CommandBuffer cmd, uint32_t currentImage) override;

    void bindBatchData(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<RenderObjectsDataManager> batchData) override;

    void setupGlobalDescriptor(std::shared_ptr<RenderObjectsDataManager> dataManager) override;

    [[nodiscard]] std::shared_ptr<GraphicsRenderPipeline> getRenderPipeline() const;

    void setCameraMatrix(glm::mat4 cameraMatrix, glm::vec3 camPos);

    void setupMesh(std::shared_ptr<Mesh> mesh) override;

    bool processMeshes() override;


    void destroy() override;
};



#endif //GBUFFERPIPELINE_HPP

#ifndef PBRPIPELINE_H
#define PBRPIPELINE_H

#include "Pipelines/Standart/GBufferPipeline.hpp"
#include "VulkanLib/Device/Buffer/StorageBuffer.hpp"
#include <cstdint>
#include <memory>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

#include "Pipelines/IPipelineGraphics.hxx"
#include <VulkanLib/Device/Buffer/PushConstant.hpp>
#include <RenderData/DescriptorSetData.hpp>


struct PointLight{
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
};

struct DirectLight{
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
};

struct LighConfig{
    int enabledDirects = 0;
    int enabledPoints = 0;


    float emissiveIntensity = 2;
    float emissiveShininess = 1;
    float gammaCorrect = 1.0f / 2.2f;
    float ambientIntensity = 0.003f;
    glm::vec3 cameraPosition;
};


class PBRPipeline: public IPipelineGraphics, public IResizeCallback{
public:
    PBRPipeline(const std::shared_ptr<LogicalDevice>& device, std::shared_ptr<GBufferPipeline> gbPipeline, uint32_t maxPointLighsAmount, uint32_t maxDirectLightBlocks);
private:
    std::shared_ptr<LogicalDevice> device;
    std::shared_ptr<DescriptorPool> descriptorPool;
    std::shared_ptr<GraphicsRenderPipeline> renderPipeline;
    uint32_t pipelineId;
    uint32_t framesInFlight;
    std::shared_ptr<PushConstant> pushConstant;
    std::pair<std::shared_ptr<IndexBuffer>, std::shared_ptr<VertexBuffer>> quadMesh;
    std::shared_ptr<DescriptorSetData> descriptorSetData;
    std::shared_ptr<StorageBuffer> pointLightsStorage;
    std::shared_ptr<StorageBuffer> directLightsStorage;
    uint32_t maxPointLighsAmount; 
    uint32_t maxDirectLightBlocks;
    std::shared_ptr<GBufferPipeline> gbPipeline;
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

    PointLight* getPointLightBlock(uint32_t index);

    DirectLight* getDirectLightBlock(uint32_t index);

    LighConfig* getLightConfiguration();

    void bindBatchData(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<RenderObjectsDataManager> batchData) override;

    void setupGlobalDescriptor(std::shared_ptr<RenderObjectsDataManager> dataManager) override;

    [[nodiscard]] std::shared_ptr<GraphicsRenderPipeline> getRenderPipeline() const;

    void setCameraPosition(glm::vec3 camPos);

    void setupMesh(std::shared_ptr<Mesh> mesh) override;

    bool processMeshes() override;


    void destroy() override;
};

#endif // PBRPIPELINE_H

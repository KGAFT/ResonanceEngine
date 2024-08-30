#pragma once

#include "VulkanLib/Device/Descriptors/DescriptorPool.hpp"
#include "VulkanLib/Device/Synchronization/IResizeCallback.hpp"
#include "VulkanLib/Shader/ShaderCreateInfo.hpp"
#include "VulkanLib/RenderPipeline/GraphicsRenderPipeline.hpp"
#include <memory>
#include <vulkan/vulkan_handles.hpp>

#include "RenderData/RenderObjectsDataManager.hpp"

class IPipelineGraphics : public IDestroyableObject, public IResizeCallback {
public:
    virtual void populateBuilder(std::shared_ptr<RenderPipelineBuilder> builder) = 0;
    virtual std::shared_ptr<std::vector<ShaderCreateInfo>> getShadersInfos() = 0;
    virtual void setDescriptorPool(std::shared_ptr<DescriptorPool> descriptorPool) = 0;
    virtual void setRenderPipeline(std::shared_ptr<GraphicsRenderPipeline> renderPipeline) = 0;
    virtual bool attachToSwapChain() = 0;
    virtual void setMaxFramesInFlight(uint32_t framesInFlight) = 0;
    virtual void setupGlobalDescriptor(std::shared_ptr<RenderObjectsDataManager> dataManager) = 0;
    //Will be called after all set commands was executed
    virtual void setup() = 0;
    virtual void beginRender(vk::CommandBuffer cmd, uint32_t currentImage) = 0;
    virtual void bindBatchData(vk::CommandBuffer cmd, uint32_t currentImage, std::shared_ptr<RenderObjectsDataManager> batchData) = 0;
    virtual void render(vk::CommandBuffer cmd, uint32_t currentImage,
                        std::shared_ptr<Mesh> mesh, std::shared_ptr<IndirectBuffer> indirectBuffer, size_t indirectOffset) = 0;
    virtual void render(vk::CommandBuffer cmd, uint32_t currentImage) = 0;

    virtual void endRender(vk::CommandBuffer cmd, uint32_t currentImage) = 0;
    virtual void setupMesh(std::shared_ptr<Mesh> mesh) = 0;
    virtual bool processMeshes() = 0;
    virtual void resized(uint32_t width, uint32_t height) = 0;
    virtual void destroy() = 0;
};

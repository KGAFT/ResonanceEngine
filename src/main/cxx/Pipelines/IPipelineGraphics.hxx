#pragma once

#include "VulkanLib/Device/Descriptors/DescriptorPool.hpp"
#include "VulkanLib/Device/Synchronization/IResizeCallback.hpp"
#include "VulkanLib/Shader/ShaderCreateInfo.hpp"
#include "VulkanLib/RenderPipeline/GraphicsRenderPipeline.hpp"
#include <memory>
#include <vulkan/vulkan_handles.hpp>

class IPipelineGraphics : public IDestroyableObject, public IResizeCallback {
public:
    virtual void populateBuilder(std::shared_ptr<RenderPipelineBuilder> builder) = 0;
    virtual std::shared_ptr<std::vector<ShaderCreateInfo>> getShadersInfos() = 0;
    virtual void setup(std::shared_ptr<DescriptorPool> descriptorPool) = 0;
    virtual void render(vk::CommandBuffer cmd, uint32_t currentImg) = 0;
    virtual void resized(uint32_t width, uint32_t height) = 0;

};
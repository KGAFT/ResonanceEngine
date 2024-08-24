#pragma once

#include "VulkanLib/Device/Synchronization/IResizeCallback.hpp"
#include "VulkanLib/Shader/ShaderCreateInfo.hpp"
#include "VulkanLib/RenderPipeline/GraphicsRenderPipeline.hpp"

class IPipelineGraphics : public IDestroyableObject, public IResizeCallback {
public:
    virtual void populateBuilder(std::shared_ptr<RenderPipelineBuilder> builder) = 0;
    virtual std::shared_ptr<std::vector<ShaderCreateInfo>> getShadersInfos() = 0;
};
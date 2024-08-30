//
// Created by kgaft on 8/30/24.
//

#ifndef RAYTRACINGPIPELINE_HPP
#define RAYTRACINGPIPELINE_HPP
#include "RenderData/RenderObjectsDataManager.hpp"
#include "VulkanLib/Device/Descriptors/DescriptorSet.hpp"
#include "VulkanLib/Device/LogicalDevice/LogicalDevice.hpp"
#include "VulkanLib/Pipelines/RayTracingPipeline/RayTracingPipelineBuilder.hpp"
#include "VulkanLib/RenderPipeline/RayTracingRenderPipeline.hpp"
#include "VulkanLib/Shader/ShaderCreateInfo.hpp"
#include "VulkanLib/Device/Descriptors/DescriptorPool.hpp"

class RayTracingPipeline {

public:
    RayTracingPipeline(std::shared_ptr<LogicalDevice> &device, Instance &instance, std::shared_ptr<vkLibRt::RayTracingPipelineBuilder>& builder, std::vector<ShaderCreateInfo>& shadersInfos);
protected:
    RayTracingPipeline(std::shared_ptr<LogicalDevice> &device, Instance &instance) : device(device), instance(instance){}
private:
    std::shared_ptr<DescriptorSet> pipelineDescriptor;
    std::shared_ptr<vkLibRt::RayTracingRenderPipeline> renderPipeline;
    std::shared_ptr<LogicalDevice> device;
    std::shared_ptr<Image> renderOutput;
protected:
    std::shared_ptr<vkLibRt::RayTracingPipelineBuilder> builder;
    std::vector<ShaderCreateInfo> createInfos;
    std::shared_ptr<RenderObjectsDataManager> dataManager;
    Instance& instance;
    Shader* shader;
public:
    [[nodiscard]] std::vector<ShaderCreateInfo> getCreateInfos() const;
    void setShader(Shader *newShader);
    void setup(std::shared_ptr<DescriptorPool> descriptorPool, uint32_t width, uint32_t height);
    virtual void beginRayTrace(vk::CommandBuffer cmd);
    virtual void rayTrace(vk::CommandBuffer cmd, uint32_t width, uint32_t height);
    void processRenderObjects(std::shared_ptr<RenderObjectsDataManager> dataManager);
    void resize(uint32_t width, uint32_t height);

public:
    const std::shared_ptr<vkLibRt::RayTracingRenderPipeline> &getRenderPipeline() const;

public:
    const std::shared_ptr<Image> &getRenderOutput() const;

public:
    const std::shared_ptr<DescriptorSet> &getPipelineDescriptor() const;
};



#endif //RAYTRACINGPIPELINE_HPP

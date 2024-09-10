//
// Created by kgaft on 8/30/24.
//

#ifndef PIPELINEMANAGER_HPP
#define PIPELINEMANAGER_HPP
#include "IPipelineGraphics.hxx"
#include "RayTracingPipeline.hpp"
#include "Settings/SettingsManager.hxx"
#include "Vulkan/VulkanContext.hpp"
#include "VulkanLib/Shader/ShaderLoader.hpp"


class PipelineManager : public IDestroyableObject, public IResizeCallback{
public:
    PipelineManager() {
        VulkanContext::getSyncManager()->addResizeCallback(this);
        shaderLoaderInstance->getIncludeDirectories().push_back("shaders/");
    }

private:
    std::vector<std::shared_ptr<IPipelineGraphics>> graphicsPipelines;
    std::vector<std::shared_ptr<RayTracingPipeline>> rayTracingPipelines;
    std::shared_ptr<LogicalDevice> device = VulkanContext::getDevice();
    std::shared_ptr<IPipelineGraphics> presentationPipeline;
    std::shared_ptr<RenderPipelineBuilder> builder = std::make_shared<RenderPipelineBuilder>();
    ShaderLoader* shaderLoaderInstance = ShaderLoader::getInstance();
    std::vector<Shader*> loadedShaders;
public:
    void addGraphicsPipeline(std::shared_ptr<IPipelineGraphics> pipeline) {
        graphicsPipelines.push_back(pipeline);
        initializePipeline(pipeline);
    }
    void setPresentationPipeline(std::shared_ptr<IPipelineGraphics> pipeline) {
        this->presentationPipeline = pipeline;
        initializePipeline(pipeline);
    }
    void addRayTracingPipeline(std::shared_ptr<RayTracingPipeline> rtPipeline){
        auto resolution = *SettingsManager::getInstance()->getSetting<RESOLUTION_INFO_T>(RESOLUTION_INFO) * *SettingsManager::getInstance()->getSetting<RESOLUTION_INFO_T>(RESOLUTION_INFO);
        rayTracingPipelines.push_back(rtPipeline);
        Shader *shader = shaderLoaderInstance->createShader(*VulkanContext::getDevice(), rtPipeline->createInfos);
        rtPipeline->setShader(shader);
        rtPipeline->setup(VulkanContext::getDescriptorPool(), resolution.x, resolution.y);
    }

    void draw(std::shared_ptr<RenderObjectsDataManager> renderObjects, std::shared_ptr<Model> model) {
        uint32_t curCmd;
        auto cmd = VulkanContext::getSyncManager()->beginRender(curCmd);
        for (auto gPipeline : graphicsPipelines) {
            gPipeline->beginRender(cmd, 0);
            gPipeline->bindBatchData(cmd, 0, renderObjects);
            for (auto mesh : model->getMeshes()) {
                gPipeline->render(cmd, 0, mesh, renderObjects->getIndirectBuffer(), mesh->getIndirectOffset());
            }
            gPipeline->endRender(cmd, 0);
        }
        presentationPipeline->beginRender(cmd, curCmd);
        presentationPipeline->render(cmd, curCmd);
        presentationPipeline->endRender(cmd, curCmd);
        VulkanContext::getSyncManager()->endRender();
    }

    void resized(uint32_t width, uint32_t height) override {
        for (auto graphicsPipeline : graphicsPipelines) {
            graphicsPipeline->resized(width, height);
        }
        for (auto rayTracingPipeline : rayTracingPipelines) {
            rayTracingPipeline->resize(width, height);
        }
        presentationPipeline->resized(width, height);
    }

private:
    void initializePipeline(std::shared_ptr<IPipelineGraphics> pipeline) {
        try {
            builder->clear();
            pipeline->populateBuilder(builder);
            auto shadersInfos = pipeline->getShadersInfos();
            Shader *shader = shaderLoaderInstance->createShader(*VulkanContext::getDevice(), *shadersInfos);
            loadedShaders.push_back(shader);
            std::shared_ptr<GraphicsRenderPipeline> res;
            auto resolution = *SettingsManager::getInstance()->getSetting<RESOLUTION_INFO_T>(RESOLUTION_INFO);

            if (!pipeline->attachToSwapChain()) {
                resolution*=*SettingsManager::getInstance()->getSetting<RENDER_SCALE_INFO_T>(RENDER_SCALE_INFO);
                res = std::make_shared<GraphicsRenderPipeline>(VulkanContext::getVulkanInstance(),
                                                               VulkanContext::getDevice(),
                                                               builder.get(),
                                                               shader,
                                                               vk::Extent2D{
                                                                       static_cast<uint32_t>(resolution.x),
                                                                       static_cast<uint32_t>(resolution.y)},
                                                              1);
            } else {
                res = std::make_shared<GraphicsRenderPipeline>(VulkanContext::getVulkanInstance(), VulkanContext::getDevice(), VulkanContext::getSwapChain(), builder.get(),
                                                               shader,
                                                               vk::Extent2D{
                                                                       static_cast<uint32_t>(resolution.x),
                                                                       static_cast<uint32_t>(resolution.y)},
                                                                VulkanContext::getMaxFramesInFlight());
            }

            pipeline->setRenderPipeline(res);
            pipeline->setDescriptorPool(VulkanContext::getDescriptorPool());
            pipeline->setMaxFramesInFlight(pipeline->attachToSwapChain()?VulkanContext::getMaxFramesInFlight():1);
            pipeline->setup();
        } catch (std::exception &exception) {
            std::cerr << exception.what() << std::endl;
        }
    }

protected:
    void destroy() override {
        for (auto graphicsPipeline : graphicsPipelines) {
            graphicsPipeline->destroy();
        }
        presentationPipeline->destroy();
        for (auto shader : loadedShaders) {
            shader->destroy();
        }
    }
};



#endif //PIPELINEMANAGER_HPP

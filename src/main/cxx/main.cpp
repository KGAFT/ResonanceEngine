#include <iostream>

#include "Pipelines/OutputPipeline.hpp"
#include "Settings/SettingsManager.hxx"
#include "Settings/SettingsResizeCallback.hxx"
#include "Vulkan/VulkanContext.hpp"
#include "VulkanLib/Device/DeviceBuilder.hpp"
#include "Window/Window.hxx"
#include "Verbose/EngineLogger.hxx"
#include "Verbose/DevicePicker/DevicePickerConsole.hxx"
#include "VulkanLib/Shader/ShaderLoader.hpp"


int main() {
    auto window = Window::createWindow(1280, 720, "Hello world");
    SettingsManager::getInstance()->storeData<RESOLUTION_INFO_T>(RESOLUTION_INFO, glm::vec2(1280, 720));
    SettingsManager::getInstance()->storeData<RENDER_SCALE_INFO_T>(RENDER_SCALE_INFO, 1.0f);
    window->addResizeCallback(new SettingsResizeCallback(SettingsManager::getInstance()));
    auto logger = EngineLogger::getLoggerInstance();
    DevicePickerConsole picker;
    uint32_t deviceCount;
    DeviceBuilder builder;
    builder.requestGraphicsSupport();
    builder.requestRayTracingSupport();
    builder.requestPresentSupport(window->getWindowSurface(VulkanContext::getVulkanInstance().getInstance()));
    auto devices = VulkanContext::enumerateSupportedDevices(
        builder, window->getWindowSurface(VulkanContext::getVulkanInstance().getInstance()), &deviceCount);

    auto result = picker.pickDevice(devices, deviceCount);
    VulkanContext::pickDevice(builder, result,
                              window->getWindowSurface(VulkanContext::getVulkanInstance().getInstance()),
                              window->getWidth(), window->getHeight());
    std::shared_ptr<RenderPipelineBuilder> rBuilder = std::make_shared<RenderPipelineBuilder>();
    std::shared_ptr<OutputPipeline> pipeline = std::make_shared<OutputPipeline>(VulkanContext::getDevice());


    std::vector<std::shared_ptr<Image>> usedImages;

    for(uint32_t i = 0; i < VulkanContext::getMaxFramesInFlight(); i++) {
        auto imageF = GraphicsRenderPipeline::getRenderImagePool(VulkanContext::getDevice())->acquireColorRenderImage(800, 600);
        auto imageS = GraphicsRenderPipeline::getRenderImagePool(VulkanContext::getDevice())->acquireColorRenderImage(800, 600);
        auto imageT = GraphicsRenderPipeline::getRenderImagePool(VulkanContext::getDevice())->acquireColorRenderImage(800, 600);
        pipeline->setMaxFramesInFlight(VulkanContext::getMaxFramesInFlight());
        pipeline->addAttachment(imageF->getImageViews()[0]);
        pipeline->addAttachment(imageS->getImageViews()[0]);
        pipeline->addAttachment(imageT->getImageViews()[0]);
        usedImages.push_back(imageF);
        usedImages.push_back(imageS);
        usedImages.push_back(imageT);
    }

    pipeline->setMaxFramesInFlight(VulkanContext::getMaxFramesInFlight());

    rBuilder->clear();
    pipeline->populateBuilder(rBuilder);
    auto shadersInfos = pipeline->getShadersInfos();
    Shader *shader = ShaderLoader::getInstance()->createShader(*VulkanContext::getDevice(), *shadersInfos);
    std::shared_ptr<GraphicsRenderPipeline> res;
    if (!pipeline->attachToSwapChain()) {
        res = std::make_shared<GraphicsRenderPipeline>(VulkanContext::getVulkanInstance(),
                                                       VulkanContext::getDevice(),
                                                       rBuilder.get(),
                                                       shader,
                                                       vk::Extent2D{
                                                           static_cast<uint32_t>(window->getWidth() * 4),
                                                           static_cast<uint32_t>(window->getHeight() * 4)
                                                       },
                                                       VulkanContext::getMaxFramesInFlight());
    } else {
        res = std::make_shared<GraphicsRenderPipeline>(VulkanContext::getVulkanInstance(), VulkanContext::getDevice(),
                                                       VulkanContext::getSwapChain(), rBuilder.get(),
                                                       shader,
                                                       vk::Extent2D{
                                                           static_cast<uint32_t>(window->getWidth()),
                                                           static_cast<uint32_t>(window->getHeight())
                                                       },
                                                       VulkanContext::getMaxFramesInFlight());
    }

    pipeline->setRenderPipeline(res);
    pipeline->setDescriptorPool(VulkanContext::getDescriptorPool());


    pipeline->setup();


    VulkanContext::getSyncManager()->addResizeCallback(pipeline);
    window->addResizeCallback(VulkanContext::getSyncManager().get());
    window->enableRefreshRateInfo();
    while (!window->needToClose()) {
        window->preRenderEvents();
        uint32_t curCmd;
        auto cmd = VulkanContext::getSyncManager()->beginRender(curCmd);
        pipeline->beginRender(cmd, curCmd);
        pipeline->render(cmd, curCmd);
        pipeline->endRender(cmd, curCmd);
        VulkanContext::getSyncManager()->endRender();
        window->postRenderEvents();
    }
    return 0;
}

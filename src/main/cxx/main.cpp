#include <iostream>

#include "Pipelines/Standart/OutputPipeline.hpp"
#include "Pipelines/PipelineManager.hpp"
#include "Settings/SettingsManager.hxx"
#include "Settings/SettingsResizeCallback.hxx"
#include "Vulkan/VulkanContext.hpp"
#include "VulkanLib/Device/DeviceBuilder.hpp"
#include "Window/Window.hxx"
#include "Verbose/EngineLogger.hxx"
#include "Verbose/DevicePicker/DevicePickerConsole.hxx"
#include "VulkanLib/Shader/ShaderLoader.hpp"
#include <Pipelines/Standart/GBufferPipeline.hpp>
#include <Assets/AssetImporter.hpp>

#include "Camera/CameraManager.hxx"

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
    //builder.requestRayTracingSupport();
    builder.requestPresentSupport(window->getWindowSurface(VulkanContext::getVulkanInstance(false).getInstance()));
    auto devices = VulkanContext::enumerateSupportedDevices(
        builder, window->getWindowSurface(VulkanContext::getVulkanInstance().getInstance()), &deviceCount);

    auto result = picker.pickDevice(devices, deviceCount);
    VulkanContext::pickDevice(builder, result,
                              window->getWindowSurface(VulkanContext::getVulkanInstance().getInstance()),
                              window->getWidth(), window->getHeight());

    AssetImporter importer(VulkanContext::getDevice(), false);
    auto model = importer.loadModel("assets/Sponza/glTF/Sponza.gltf");
    auto renderData = importer.makeBatchData();
    std::shared_ptr<OutputPipeline> pipeline = std::make_shared<OutputPipeline>(VulkanContext::getDevice());
    std::shared_ptr<GBufferPipeline> gPipeline = std::make_shared<GBufferPipeline>(VulkanContext::getDevice(), renderData);



    pipeline->setMaxFramesInFlight(VulkanContext::getMaxFramesInFlight());

    PipelineManager pipelineManager;
    pipelineManager.addGraphicsPipeline(gPipeline);
    for(uint32_t i = 0; i < VulkanContext::getMaxFramesInFlight(); i++) {

        pipeline->setMaxFramesInFlight(VulkanContext::getMaxFramesInFlight());
        pipeline->addAttachment(gPipeline->getRenderPipeline()->getBaseRenderImages()[4*i+0]->getImageViews()[0]);
        pipeline->addAttachment(gPipeline->getRenderPipeline()->getBaseRenderImages()[4*i+1]->getImageViews()[0]);
        pipeline->addAttachment(gPipeline->getRenderPipeline()->getBaseRenderImages()[4*i+2]->getImageViews()[0]);

    }
    gPipeline->setupGlobalDescriptor(renderData);
    pipelineManager.setPresentationPipeline(pipeline);

    window->addResizeCallback(VulkanContext::getSyncManager().get());
    window->enableRefreshRateInfo();
    window->getInputSystem().registerKeyCallback(pipeline.get());
    CameraManager cameraManager(window);

    while (!window->needToClose()) {
        window->preRenderEvents();
        glm::mat4 camMatrix = cameraManager.getCurrentCamera()->calculateCameraMatrix( 75, 0.05f, 2600000, (float)window->getWidth()/(float)window->getHeight());
        glm::vec3 camPos;
        cameraManager.getCurrentCamera()->getPosition(camPos);
        gPipeline->setCameraMatrix(camMatrix, camPos);
        pipelineManager.draw(renderData, model);
        window->postRenderEvents();
    }
    return 0;
}

#include <iostream>

#include "Pipelines/Standart/OutputPipeline.hpp"
#include "Pipelines/PipelineManager.hpp"
#include "Pipelines/Standart/PBRPipeline.h"
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
#include <memory>

#include "Camera/CameraManager.hxx"

int main() {
    auto window = Window::createWindow(1920, 1080, "Hello world");
    SettingsManager::getInstance()->storeData<RESOLUTION_INFO_T>(RESOLUTION_INFO, glm::vec2(1920, 1080));
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
    auto model = importer.loadModel("assets/DamagedHelmet/glTF/DamagedHelmet.gltf");
    auto renderData = importer.makeBatchData();
    std::shared_ptr<OutputPipeline> pipeline = std::make_shared<OutputPipeline>(VulkanContext::getDevice());
    std::shared_ptr<GBufferPipeline> gPipeline = std::make_shared<GBufferPipeline>(VulkanContext::getDevice(), renderData);
    auto pbrPipeline = std::make_shared<PBRPipeline>(VulkanContext::getDevice(), gPipeline, 128, 128);


    pipeline->setMaxFramesInFlight(VulkanContext::getMaxFramesInFlight());

    PipelineManager pipelineManager;
    pipelineManager.addGraphicsPipeline(gPipeline);
    pipelineManager.addGraphicsPipeline(pbrPipeline);
    for(uint32_t i = 0; i < VulkanContext::getMaxFramesInFlight(); i++) {
        pipeline->addAttachment(pbrPipeline->getRenderPipeline()->getBaseRenderImages()[0]->getImageViews()[0]);
        pipeline->addAttachment(pbrPipeline->getRenderPipeline()->getBaseRenderImages()[0]->getImageViews()[0]);
        pipeline->addAttachment(pbrPipeline->getRenderPipeline()->getBaseRenderImages()[0]->getImageViews()[0]);
    }
    gPipeline->setupGlobalDescriptor(renderData);
    pipelineManager.setPresentationPipeline(pipeline);
    window->addResizeCallback(VulkanContext::getSyncManager().get());
    window->enableRefreshRateInfo();
    window->getInputSystem().registerKeyCallback(pipeline.get());
    pbrPipeline->getLightConfiguration()->enabledPoints = 1;
    pbrPipeline->getLightConfiguration()->enabledDirects = 0;
    pbrPipeline->getLightConfiguration()->ambientIntensity = 0.03;
    pbrPipeline->getPointLightBlock(0)->position = glm::vec3(2, 3, 2);
    pbrPipeline->getPointLightBlock(0)->color = glm::vec3(1,0,0);
    pbrPipeline->getPointLightBlock(0)->intensity = 100;
    CameraManager cameraManager(window);
    cameraManager.getCurrentCamera()->getPosition(pbrPipeline->getLightConfiguration()->cameraPosition);
    while (!window->needToClose()) {
        window->preRenderEvents();
        glm::mat4 camMatrix = cameraManager.getCurrentCamera()->calculateCameraMatrix( 75, 0.05f, 2600000, (float)window->getWidth()/(float)window->getHeight());
        glm::vec3 camPos;
        cameraManager.getCurrentCamera()->getPosition(camPos);

        gPipeline->setCameraMatrix(camMatrix, camPos);
        cameraManager.getCurrentCamera()->getPosition(pbrPipeline->getLightConfiguration()->cameraPosition);
        pipelineManager.draw(renderData, model);
        
        window->postRenderEvents();
    }
    gPipeline->destroy();
    pbrPipeline->destroy();
    pipeline->destroy();
    VulkanContext::shutDown();
    return 0;
}

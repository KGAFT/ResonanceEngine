#include <iostream>
#include "Settings/SettingsManager.hxx"
#include "Settings/SettingsResizeCallback.hxx"
#include "Vulkan/VulkanContext.hpp"
#include "VulkanLib/Device/DeviceBuilder.hpp"
#include "Window/Window.hxx"
#include "Verbose/EngineLogger.hxx"
#include "Verbose/DevicePicker/DevicePickerConsole.hxx"


int main(){
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
    auto devices = VulkanContext::enumerateSupportedDevices(builder, window->getWindowSurface(VulkanContext::getVulkanInstance().getInstance()), &deviceCount);

    auto result = picker.pickDevice(devices, deviceCount);
    VulkanContext::pickDevice(builder, result, window->getWindowSurface(VulkanContext::getVulkanInstance().getInstance()), window->getWidth(), window->getHeight());
    
    while(!window->needToClose()){
        window->preRenderEvents();
        window->postRenderEvents();
    }
    return 0;
}
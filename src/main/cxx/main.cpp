#include <iostream>
#include "Vulkan/VulkanContext.hpp"
#include "VulkanLib/Device/DeviceBuilder.hpp"
#include "Window/Window.hxx"
#include "Verbose/EngineLogger.hxx"
#include "Verbose/DevicePicker/DevicePickerConsole.hxx"


int main(){
    auto window = Window::createWindow(1920, 1080, "Hello world");
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
    

    return 0;
}
#include "VulkanLib/Device/DeviceBuilder.hpp"
#include "com_kgaft_ResonanceEngine_Native_NativeMain.h"

#include <iostream>
#include <SDL3/SDL.h>
#include <Window/Window.hxx>
#include <VulkanLib/Instance.hpp>
#include <VulkanLib/InstanceLogger/DefaultVulkanFileLoggerCallback.hpp>

#include "VulkanLib/Device/DeviceBuilder.hpp"
#include "VulkanLib/Device/LogicalDevice/LogicalDevice.hpp"
#include <map>
#include <VulkanLib/Device/SwapChain/SwapChain.hpp>
#include <assimp/Importer.hpp>
//example code just to ensure that all modules is working properly!

JNIEXPORT void JNICALL Java_com_kgaft_ResonanceEngine_Native_NativeMain_sayHello
  (JNIEnv *env, jclass curClass) {
    Assimp::Importer importer;
    auto window = Window::createWindow(1280, 720, "Native window!");
    InstanceBuilder instanceBuilder;
    instanceBuilder.setApplicationName("VulkanLib testing app");
    
    instanceBuilder.presetForDebug();
    std::vector<const char*> extensions;
    Window::getRequiredExtensions(extensions);
    instanceBuilder.addExtensions(extensions.data(), extensions.size());

    instanceBuilder.addLoggerCallback(new DefaultVulkanLoggerCallback);
    instanceBuilder.addLoggerCallback(new DefaultVulkanFileLoggerCallback("log.txt"));
    Instance instance(instanceBuilder);
    DeviceBuilder builder;
    builder.requestGraphicsSupport();
    builder.requestComputeSupport();
    builder.requestRayTracingSupport();
    builder.requestPresentSupport(window->getWindowSurface(instance.getInstance()));
    auto devices = PhysicalDevice::getDevices(instance);
    std::map<std::shared_ptr<PhysicalDevice>, DeviceSuitabilityResults*> supportedDevices;
    for(auto device : *devices){
        auto results = new DeviceSuitabilityResults;
        if(DeviceSuitability::isDeviceSuitable(builder, device, results)){
            supportedDevices[device] = results;
        }
    }
    for(auto device : supportedDevices){
        std::cout<<device.first->getProperties().deviceName<<std::endl;
    }
    auto device = std::make_shared<LogicalDevice>(instance, (*devices)[0], builder, supportedDevices[(*devices)[0]]);
    auto swapChain = std::make_shared<SwapChain>(device, window->getWindowSurface(instance.getInstance()), window->getWidth(), window->getHeight(), false);
   
    std::cout<<"Hello native!"<<std::endl;
}
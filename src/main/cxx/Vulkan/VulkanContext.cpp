#include "VulkanContext.hpp"
#include "VulkanLib/Device/DeviceBuilder.hpp"
#include "Verbose/EngineLogger.hxx"
#include <Window/Window.hxx>

//#include "Verbose/EngineLogger.hxx"

Instance &VulkanContext::getVulkanInstance(bool enableGc) {
    if (!apiInitialized) {
        InstanceBuilder instanceBuilder;
        instanceBuilder.enableGarbageCollector(enableGc);
        std::vector<const char*> extensions;
        Window::getRequiredExtensions(extensions);

        for (uint32_t i = 0; i < extensions.size(); ++i) {
            instanceBuilder.addExtension(extensions[i]);
        }
        if (Instance::debugSupported()) {
            instanceBuilder.presetForDebug();
            instanceBuilder.addLoggerCallback(EngineLogger::getVulkanCallback());
        }
        instance = Instance(instanceBuilder);
        apiInitialized = true;
    }
    return instance;
}

void VulkanContext::pickDevice(DeviceBuilder& deviceBuilder, uint32_t deviceIndex, VkSurfaceKHR surfaceKhr, uint32_t startWidth,
                               uint32_t startHeight) {
    if (deviceIndex >= supportedDevices.size()) {
        throw std::runtime_error("Error out of bound device index: " + deviceIndex);
    }
    if (!device) {
        device = std::make_shared<LogicalDevice>(instance, supportedDevices[deviceIndex], deviceBuilder,
                                                 &deviceSuitabilities[deviceIndex]);
        swapChain = std::make_shared<SwapChain>(device, surfaceKhr, startWidth, startHeight,  false);
        syncManager = std::make_shared<SyncManager>(device, swapChain, device->getPresentQueue(),
                                                    swapChain->getSwapchainImageViews().size());
        descriptorPool = DescriptorPool::getInstance(device, deviceBuilder.rayTracingSupport1());
    }
}

const char **VulkanContext::enumerateSupportedDevices(DeviceBuilder& deviceBuilder, VkSurfaceKHR surfaceKhr, uint32_t *pDeviceCount) {
    if (supportedDevices.empty()) {

        auto allDevices = PhysicalDevice::getDevices(instance);
        for (auto &item: (*allDevices)) {
            if (DeviceSuitability::isDeviceSuitable(deviceBuilder, item, nullptr)) {
                supportedDevices.push_back(item);
                deviceSuitabilities.push_back(DeviceSuitabilityResults());
                DeviceSuitability::isDeviceSuitable(deviceBuilder, item,
                                                    &deviceSuitabilities[deviceSuitabilities.size() - 1]);
                deviceNames.push_back(item->properties.deviceName);
            }
        }
    }
    *pDeviceCount = deviceNames.size();
    return deviceNames.data();
}

std::shared_ptr<LogicalDevice> VulkanContext::getDevice() {
    return device;
}

uint32_t VulkanContext::getMaxFramesInFlight() {
    return swapChain->getSwapchainImageViews().size();
}

const std::shared_ptr<SwapChain> &VulkanContext::getSwapChain() {
    return swapChain;
}

const std::shared_ptr<DescriptorPool> &VulkanContext::getDescriptorPool() {
    return descriptorPool;
}

const std::shared_ptr<SyncManager> &VulkanContext::getSyncManager() {
    return syncManager;
}

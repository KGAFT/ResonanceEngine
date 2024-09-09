#ifndef RESONANCEENGINE_VULKANCONTEXT_HPP
#define RESONANCEENGINE_VULKANCONTEXT_HPP


#include "VulkanLib/Instance.hpp"
#include "VulkanLib/Device/DeviceBuilder.hpp"
#include "VulkanLib/Device/PhysicalDevice/PhysicalDevice.hpp"
#include "VulkanLib/Device/PhysicalDevice/DeviceSuitability.hpp"
#include "VulkanLib/Device/LogicalDevice/LogicalDevice.hpp"
#include "VulkanLib/Device/SwapChain/SwapChain.hpp"
#include "VulkanLib/Device/Synchronization/SyncManager.hpp"
#include "VulkanLib/Device/Descriptors/DescriptorPool.hpp"


class VulkanContext {
public:
    static Instance& getVulkanInstance(bool enableGc = false);
    static const char** enumerateSupportedDevices(DeviceBuilder& deviceBuilder, VkSurfaceKHR surfaceKhr, uint32_t* pDeviceCount);
    static void pickDevice(DeviceBuilder& deviceBuilder, uint32_t deviceIndex, VkSurfaceKHR surfaceKhr, uint32_t startWidth, uint32_t startHeight);
    static std::shared_ptr<LogicalDevice> getDevice();
    static uint32_t getMaxFramesInFlight();

    static const std::shared_ptr<SwapChain> &getSwapChain();

    static const std::shared_ptr<DescriptorPool> &getDescriptorPool();

    static const std::shared_ptr<SyncManager> &getSyncManager();

private:
    static inline std::shared_ptr<LogicalDevice> device = std::shared_ptr<LogicalDevice>();
    static inline std::shared_ptr<SwapChain> swapChain = std::shared_ptr<SwapChain>();
    static inline std::shared_ptr<SyncManager> syncManager = std::shared_ptr<SyncManager>();
    static inline std::shared_ptr<DescriptorPool> descriptorPool = std::shared_ptr<DescriptorPool>();

    static inline Instance instance = Instance();
    static inline bool apiInitialized = false;

    static inline std::vector<std::shared_ptr<PhysicalDevice>> supportedDevices = std::vector<std::shared_ptr<PhysicalDevice>>();
    static inline std::vector<DeviceSuitabilityResults> deviceSuitabilities = std::vector<DeviceSuitabilityResults>();
    static inline std::vector<const char*> deviceNames = std::vector<const char*>();

};


#endif //RESONANCEENGINE_VULKANCONTEXT_HPP

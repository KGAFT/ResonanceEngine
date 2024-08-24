#pragma once

#include "Settings/SettingsManager.hxx"
#include "VulkanLib/Device/Synchronization/IResizeCallback.hpp"
#include <memory>


class SettingsResizeCallback : public IResizeCallback {
public:
    SettingsResizeCallback(std::shared_ptr<SettingsManager> managerInstance) : managerInstance(managerInstance){}
private:
    std::shared_ptr<SettingsManager> managerInstance;
public:
  void resized(uint32_t width, uint32_t height) override {
    managerInstance->storeData<RESOLUTION_INFO_T>(RESOLUTION_INFO, glm::vec2(width, height));
  }
};
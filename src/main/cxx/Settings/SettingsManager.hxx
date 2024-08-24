#pragma once

#include "VulkanLib/MemoryUtils/IDestroyableObject.hpp"
#include <cstdlib>
#include <cstring>
#include <glm/glm.hpp>
#include <map>
#include <memory>
enum SettingsType { RESOLUTION_INFO, RENDER_SCALE_INFO };

#define RESOLUTION_INFO_T glm::vec2
#define RENDER_SCALE_INFO_T float

class SettingsManager : public IDestroyableObject {
private:
  static inline std::shared_ptr<SettingsManager> instance =
      std::make_shared<SettingsManager>();

public:
  static std::shared_ptr<SettingsManager> getInstance() { return instance; }

private:
  std::map<SettingsType, void *> settingsSet;

public:
  template <typename T> T *getSetting(SettingsType settingType) {
    return (T *)settingsSet[settingType];
  }
  template <typename T> void storeData(SettingsType settingType, T data) {
    void *container;
    auto entry = settingsSet.find(settingType);
    if (entry != settingsSet.end()) {
      container = entry->second;
    } else {
      container = malloc(sizeof(T));
    }
    memcpy(container, &data, sizeof(T));
    settingsSet[settingType] = container;
  }

private:
  void destroy() override {
    destroyed = true;
    for (auto &el : settingsSet) {
      free(el.second);
    }
  }
};
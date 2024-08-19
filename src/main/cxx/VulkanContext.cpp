#include "JavaPhysicalDevice.h"
#include "JavaWindow.h"
#include "VulkanLib/Device/DeviceBuilder.hpp"
#include "VulkanLib/Device/PhysicalDevice/DeviceSuitability.hpp"
#include "VulkanLib/Device/PhysicalDevice/PhysicalDevice.hpp"
#include "VulkanLib/Instance.hpp"
#include "VulkanLib/InstanceBuilder.hpp"
#include "Window/Window.hxx"
#include <com_kgaft_ResonanceEngine_Native_VulkanContext_VulkanContext.h>
#include <map>
#include <memory>

std::shared_ptr<Instance> instance;
std::map<std::shared_ptr<PhysicalDevice>,
         std::shared_ptr<DeviceSuitabilityResults>>
    supportedDevices;

void toDeviceBuilder(JNIEnv *env, jobject jbuilder, DeviceBuilder *pOutput) {
  jclass objectClass = env->GetObjectClass(jbuilder);
  jboolean requireRayTracingSupport = env->GetBooleanField(
      jbuilder, env->GetFieldID(objectClass, "requireRayTracingSupport", "Z"));
  jboolean requirePresentSupport = env->GetBooleanField(
      jbuilder, env->GetFieldID(objectClass, "requirePresentSupport", "Z"));
  pOutput->requestGraphicsSupport();
  if (requirePresentSupport) {
    jobject windowObject = env->GetObjectField(
        jbuilder, env->GetFieldID(objectClass, "requiredWindowToPresent",
                                  "Lcom/kgaft/ResonanceEngine/Native/Window"));
    auto window = GetWindow(windowObject);
    pOutput->requestPresentSupport(
        window->getWindowSurface(instance->getInstance()));
  }
  if (requireRayTracingSupport) {
    pOutput->requestRayTracingSupport();
  }
}

JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_VulkanContext_VulkanContext_initializeInstance(
    JNIEnv *env, jclass, jstring appName, jlong windowHandle) {
  InstanceBuilder instanceBuilder;
  const char *appNameC = env->GetStringUTFChars(appName, 0);
  if (windowHandle != 0) {
    auto window = (Window *)windowHandle;
    std::vector<const char *> windowExtensions;
    window->getRequiredExtensions(windowExtensions);
    instanceBuilder.addExtensions(windowExtensions.data(),
                                  windowExtensions.size());
  }
  instanceBuilder.setApplicationName(appNameC);
  instance = std::make_shared<Instance>(instanceBuilder);
  env->ReleaseStringUTFChars(appName, appNameC);
}

JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_VulkanContext_VulkanContext_enumerateSupportedDevices(
    JNIEnv *env, jclass, jobject deviceBuilder, jobject output) {
  if (instance) {
    DeviceBuilder builder;
    toDeviceBuilder(env, deviceBuilder, &builder);
    supportedDevices.clear();
    auto devices = PhysicalDevice::getDevices(*instance);

    for (auto device : *devices) {
      auto results = std::make_shared<DeviceSuitabilityResults>();
      if (DeviceSuitability::isDeviceSuitable(builder, device, results.get())) {
        supportedDevices[device] = results;
      }
    }
    for (auto device : supportedDevices) {
      env->CallObjectMethod(
          output,
          env->GetMethodID(env->GetObjectClass(output), "add",
                           "(com/kgaft/ResonanceEngine/Native/VulkanContext/"
                           "PhysicalDevice)V"),
          createPhysicalDeviceInstance(env, device.first.get()));
    }
  }
}
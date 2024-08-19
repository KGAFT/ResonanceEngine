#include "JavaPhysicalDevice.h"
#include "VulkanLib/Device/PhysicalDevice/PhysicalDevice.hpp"
#include "com_kgaft_ResonanceEngine_Native_VulkanContext_PhysicalDevice.h"
JNIEXPORT jstring JNICALL
Java_com_kgaft_ResonanceEngine_Native_VulkanContext_PhysicalDevice_getDeviceName(
    JNIEnv *env, jobject deviceObject) {
  auto device = GetPhysicalDevice(deviceObject);
  auto result = env->NewStringUTF(device->properties.deviceName);
  return result;
}
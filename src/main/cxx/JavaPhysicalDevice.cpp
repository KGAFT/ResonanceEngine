#include "JavaPhysicalDevice.h"
#include "VulkanLib/Device/PhysicalDevice/PhysicalDevice.hpp"
#include "com_kgaft_ResonanceEngine_Native_VulkanContext_PhysicalDevice.h"
#include <memory>
JNIEXPORT jstring JNICALL
Java_com_kgaft_ResonanceEngine_Native_VulkanContext_PhysicalDevice_getDeviceName(
    JNIEnv *env, jobject deviceObject) {
  auto device = GetPhysicalDevice(deviceObject);
  auto result = env->NewStringUTF(device->properties.deviceName);
  return result;
}

jobject createPhysicalDeviceInstance(JNIEnv *env, std::shared_ptr<PhysicalDevice>* base) {
  jclass clazz = env->FindClass(
      "com/kgaft/ResonanceEngine/Native/VulkanContext/PhysicalDevice");

  auto result = env->NewObject(clazz, env->GetMethodID(clazz, "<init>", "()V"));
  auto fieldId = env->GetFieldID(clazz, "deviceHandle", "J");
  env->SetLongField(result, fieldId,
                    (jlong)base);
  return result;
}
#pragma once
#include "VulkanLib/Device/PhysicalDevice/PhysicalDevice.hpp"
#include <jni.h>

#define GetPhysicalDevice(deviceObject)                                        \
  (std::shared_ptr<PhysicalDevice> *)env->GetLongField(                                         \
      deviceObject,                                                            \
      env->GetFieldID(env->GetObjectClass(deviceObject), "deviceHandle", "L"))


jobject createPhysicalDeviceInstance(JNIEnv *env, PhysicalDevice *base);


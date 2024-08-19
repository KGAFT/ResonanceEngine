#pragma once
#include "VulkanLib/Device/PhysicalDevice/PhysicalDevice.hpp"
#include <jni.h>

#define GetPhysicalDevice(deviceObject) (PhysicalDevice*)env->GetLongField(deviceObject, env->GetFieldID(env->GetObjectClass(deviceObject), "deviceHandle", "L"))

jobject createPhysicalDeviceInstance(JNIEnv* env, PhysicalDevice* base){
    jclass clazz = env->FindClass("com/kgaft/ResonanceEngine/Native/VulkanContext/PhysicalDevice");
    auto result = env->NewObject(clazz, env->GetMethodID(clazz, "<init>", "()V"));
    env->SetLongField(result, env->GetFieldID(clazz, "deviceHandle", "L"), (jlong)base);
    return result;
}
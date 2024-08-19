#pragma once
#include "VulkanLib/Device/Synchronization/IResizeCallback.hpp"
#include <jni.h>
#include <cstdint>

#define GetWindow(windowObject)                                                \
  ((Window *)env->GetLongField(                                                \
      windowObject, env->GetFieldID(env->GetObjectClass(windowObject),         \
                                    "windowHandle", "J")))

class JResizeCallback : public IResizeCallback {
public:
  JResizeCallback(JNIEnv *env, jobject callback)
      : env(env), callbackObject(callback),
        callbackId(env->GetMethodID(env->GetObjectClass(callback), "resized",
                                    "(II)V")) {
                                      
                                    }

private:
  jmethodID callbackId;
  jobject callbackObject;
  JNIEnv *env;

public:
  void resized(uint32_t width, uint32_t height) override {
    env->CallVoidMethod(callbackObject, callbackId, (jint)width, (jint)height);
  }
};
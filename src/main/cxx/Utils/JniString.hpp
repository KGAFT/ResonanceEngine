#pragma once

#include "VulkanLib/MemoryUtils/IDestroyableObject.hpp"
#include <jni.h>
#include <string>

class JNIString : public IDestroyableObject {
public:
  JNIString(JNIEnv *env, jstring string)
      : env(env), basicString(string),
        stringC(env->GetStringUTFChars(string, 0)), cppString(stringC) {}

private:
  JNIEnv *env;
  jstring basicString;
  const char *stringC;
  std::string cppString;

public:
  const char *c_str() { return stringC; }
  std::string toString() { return cppString; }
  void destroy() override {
    destroyed = true;
    env->ReleaseStringUTFChars(basicString, stringC);
  }
};
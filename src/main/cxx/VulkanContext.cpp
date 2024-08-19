#include "VulkanLib/InstanceBuilder.hpp"
#include "VulkanLib/Instance.hpp"
#include "Window/Window.hxx"
#include <com_kgaft_ResonanceEngine_Native_VulkanContext.h>
#include <memory>

std::shared_ptr<Instance> instance;

JNIEXPORT void JNICALL Java_com_kgaft_ResonanceEngine_Native_VulkanContext_initializeInstance
  (JNIEnv *env, jclass, jstring appName, jlong windowHandle){
    InstanceBuilder instanceBuilder;
    const char *appNameC = env->GetStringUTFChars(appName, 0);
    if(windowHandle!=0){
        auto window = (Window*) windowHandle;
        std::vector<const char*> windowExtensions;
        window->getRequiredExtensions(windowExtensions);
        instanceBuilder.addExtensions(windowExtensions.data(), windowExtensions.size());
    }
    instanceBuilder.setApplicationName(appNameC);
    instance = std::make_shared<Instance>(instanceBuilder);
    env->ReleaseStringUTFChars(appName, appNameC);
}
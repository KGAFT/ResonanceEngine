//
// Created by kgaft on 3/2/24.
//

#include <cstring>
#include "EngineLogger.hxx"



VulkanEngineLogCallback* EngineLogger::vulkanCallback =  new VulkanEngineLogCallback();

const std::shared_ptr<EngineLogger> &EngineLogger::getLoggerInstance() {
    if(!loggerInstance){
        loggerInstance = std::make_shared<EngineLogger>();
    }
    return loggerInstance;
}

void EngineLogger::logMessage(std::shared_ptr<EngineLogMessage> message) {
    if(message->error){
        std::cerr<<message->message<<std::endl;
    } else {
        std::cout<<message->message<<std::endl;
    }
    logFile<<message->message<<std::endl;
    logFile.flush();
}

VulkanEngineLogCallback *EngineLogger::getVulkanCallback() {
    return vulkanCallback;
}

LoggerCallbackType VulkanEngineLogCallback::getCallBackMode() {
    return TRANSLATED_DEFS;
}

void VulkanEngineLogCallback::messageRaw(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
                                         const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

}
void VulkanEngineLogCallback::translatedMessage(const char *severity, const char *type, std::string &message) {
    bool isError = !strcmp(severity, "ERROR");
    auto resMessage = std::make_shared<EngineLogMessage>("VULKAN", type, severity, message.c_str(), isError);
    EngineLogger::getLoggerInstance()->logMessage(resMessage);
}
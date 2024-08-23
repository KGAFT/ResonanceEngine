//
// Created by kgaft on 3/2/24.
//

#ifndef MENGINE_ENGINELOGGER_HXX
#define MENGINE_ENGINELOGGER_HXX

#include <VulkanLib/InstanceLogger/IInstanceLoggerCallback.hpp>
#include <memory>
#include <sstream>
#include <chrono>
#include <fstream>
#include <iostream>

class EngineLogMessage{
    friend class EngineLogger;
public:
    EngineLogMessage(const char* source, const char* type, const char* severity, const char* message, bool isError) : error(isError){
        auto currentTime = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(currentTime);
        std::string outputTime = std::string(ctime(&time));
        outputTime[outputTime.size() - 1] = ' ';
        EngineLogMessage::message = outputTime+" "+source+" ["+severity+"] "+type+" "+message;
    }
private:
    std::string message;
    bool error = false;
public:
    const std::string &getMessage() const {
        return message;
    }

    bool isError() const {
        return error;
    }
};

class VulkanEngineLogCallback;

class EngineLogger{
private:
    static inline std::shared_ptr<EngineLogger> loggerInstance = std::shared_ptr<EngineLogger>();
public:
    static const std::shared_ptr<EngineLogger> &getLoggerInstance();

    static VulkanEngineLogCallback *getVulkanCallback();

private:
    static VulkanEngineLogCallback* vulkanCallback;
public:
    EngineLogger(){
        logFile.open("MElog.txt");
    }
private:
    std::ofstream logFile;
public:
    void logMessage(std::shared_ptr<EngineLogMessage> message);
};
class VulkanEngineLogCallback : public IInstanceLoggerCallback{
    LoggerCallbackType getCallBackMode() override;

    void messageRaw(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) override;

    void translatedMessage(const char *severity, const char *type, std::string &message) override;
};




#endif //MENGINE_ENGINELOGGER_HXX

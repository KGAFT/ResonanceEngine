//
// Created by kgaft on 3/2/24.
//

#include <iostream>
#include "DevicePickerConsole.hxx"
#include "Verbose/EngineLogger.hxx"

uint32_t DevicePickerConsole::pickDevice(const char **availableDevice, uint32_t deviceCount) {
    uint32_t result = 0;
    /*
    if (deviceCount > 1) {
        auto pickTitle = std::make_shared<EngineLogMessage>("MEEngineDevicePicker", "", "",
                                                            "The system has found several graphics devices, choose one of them",
                                                            false);
        EngineLogger::getLoggerInstance()->logMessage(pickTitle);
        auto pickSTitle = std::make_shared<EngineLogMessage>("MEEngineDevicePicker", "", "", "ID\tName", false);
        for (uint32_t i = 0; i < deviceCount; i++) {
            std::ostringstream ss;
            ss << i << "\t" << availableDevice[i];
            std::string message = ss.str();
            auto deviceEntry = std::make_shared<EngineLogMessage>("", "", "", message.c_str(), false);
            EngineLogger::getLoggerInstance()->logMessage(deviceEntry);
            i++;
        }
        std::cout << "Please enter selected device id: ";
        std::cin >> result;
        if(result>=deviceCount){
            auto errorPick = std::make_shared<EngineLogMessage>("MEEngineDevicePicker", "ERROR", "ERROR",
                                                                         "Picked unexisting device, exiting",
                                                                         true);
            EngineLogger::getLoggerInstance()->logMessage(errorPick);
            throw std::runtime_error("");
        }

    }
    */
    auto pickedDeviceMesage = std::make_shared<EngineLogMessage>("MEEngineDevicePicker", "", "",
                                                                 (std::string("Picked device: ")+availableDevice[result]).c_str(),
                                                                 false);
    EngineLogger::getLoggerInstance()->logMessage(pickedDeviceMesage);
    return result;
}

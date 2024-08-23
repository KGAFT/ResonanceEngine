//
// Created by kgaft on 3/2/24.
//

#ifndef MENGINE_IDEVICEPICKER_HXX
#define MENGINE_IDEVICEPICKER_HXX


#include <cstdint>

class IDevicePicker {
public:
    virtual uint32_t pickDevice(const char** availableDevice, uint32_t deviceCount) = 0;
};


#endif //MENGINE_IDEVICEPICKER_HXX

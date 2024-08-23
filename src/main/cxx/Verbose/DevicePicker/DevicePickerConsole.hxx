//
// Created by kgaft on 3/2/24.
//

#ifndef MENGINE_DEVICEPICKERCONSOLE_HXX
#define MENGINE_DEVICEPICKERCONSOLE_HXX

#include "IDevicePicker.hxx"

class DevicePickerConsole : public IDevicePicker{
public:
    uint32_t pickDevice(const char **availableDevice, uint32_t deviceCount) override;
};


#endif //MENGINE_DEVICEPICKERCONSOLE_HXX

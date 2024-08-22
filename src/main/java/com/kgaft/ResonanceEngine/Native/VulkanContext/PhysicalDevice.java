package com.kgaft.ResonanceEngine.Native.VulkanContext;

public class PhysicalDevice {
    private long deviceHandle;

    public PhysicalDevice(long deviceHandle) {
        this.deviceHandle = deviceHandle;
    }

    public PhysicalDevice() {   
    }

    public native String getDeviceName();


}

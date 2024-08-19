package com.kgaft.ResonanceEngine.Native.VulkanContext;

import java.util.List;

public class VulkanContext {
    public static native void initializeInstance(String appName, long windowHandle);
    public static native void enumerateSupportedDevices(DeviceBuilder deviceBuilder, List<PhysicalDevice> output);
}

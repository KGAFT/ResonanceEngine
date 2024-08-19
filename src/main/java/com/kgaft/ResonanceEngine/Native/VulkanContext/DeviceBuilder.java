package com.kgaft.ResonanceEngine.Native.VulkanContext;

import com.kgaft.ResonanceEngine.Native.Window;

public class DeviceBuilder {
    private boolean requireRayTracingSupport;
    private Window requiredWindowToPresent;
    private boolean requirePresentSupport;

    public boolean isRequireRayTracingSupport() {
        return requireRayTracingSupport;
    }

    public void setRequireRayTracingSupport(boolean requireRayTracingSupport) {
        this.requireRayTracingSupport = requireRayTracingSupport;
    }

    public Window getRequiredWindowToPresent() {
        return requiredWindowToPresent;
    }

    public void setRequiredWindowToPresent(Window requiredWindowToPresent) {
        this.requiredWindowToPresent = requiredWindowToPresent;
        this.requirePresentSupport = true;
    }
}

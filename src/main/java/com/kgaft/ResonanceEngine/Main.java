package com.kgaft.ResonanceEngine;

import com.kgaft.ResonanceEngine.Native.*;
import com.kgaft.ResonanceEngine.Native.VulkanContext.VulkanContext;

public class Main {
    public static void main(String[] args) {
        ModulesInitializer.loadLibraries();
        long windowHandle = Window.createWindow(800, 600, "ResonanceEngine");
        VulkanContext.initializeInstance("ResonanceEngine", windowHandle);
        Window window = new Window(windowHandle);
        window.addResizeCallback(((width, height) -> {
            System.out.println("Resized: "+width+" "+height);
        }));
        window.enableRefreshRateInfo();
        window.setSize(1920, 1080);
        window.setTitle("Testing");
        while(!window.needToClose()){
            window.postRenderEvents();
        }
    }
}

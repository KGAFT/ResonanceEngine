package com.kgaft.ResonanceEngine;

import java.util.Scanner;

import com.kgaft.ResonanceEngine.Native.*;

public class Main {
    public static void main(String[] args) {
        ModulesInitializer.loadLibraries();
        long windowHandle = Window.createWindow(800, 600, "ResonanceEngine");
        VulkanContext.initializeInstance("ResonanceEngine", windowHandle);
        Window window = new Window(windowHandle);
        while(!window.needToClose()){
            window.postRenderEvents();
        }
    }
}

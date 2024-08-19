package com.kgaft.ResonanceEngine;

import java.util.Scanner;

import com.kgaft.ResonanceEngine.Native.*;

public class Main {
    public static void main(String[] args) {
        ModulesInitializer.loadLibraries();
        long window = Window.createWindow(800, 600, "ResonanceEngine");
        VulkanContext.initializeInstance("ResonanceEngine", window);

        Scanner scanner = new Scanner(System.in);
        scanner.nextInt();
    }
}

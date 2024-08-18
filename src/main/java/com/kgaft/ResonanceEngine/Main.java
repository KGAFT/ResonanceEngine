package com.kgaft.ResonanceEngine;

import java.util.Scanner;

import com.kgaft.ResonanceEngine.Native.NativeMain;


public class Main {
    public static void main(String[] args) {
        ModulesInitializer.loadLibraries();
        NativeMain.sayHello();
        Scanner scanner = new Scanner(System.in);
        scanner.nextInt();
    }
}

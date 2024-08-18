package com.kgaft.ResonanceEngine;

import java.io.File;
import java.util.Arrays;

enum LibraryType{
    APPLE_LIBRARY(".dylib"),
    UNIX_LIBRARY(".so"),
    WINDOWS_LIBRARY(".dll");

    private String extension;

    private LibraryType(String extension){
        this.extension = extension;
    }

    public String getExtension() {
        return extension;
    }
}

public class ModulesInitializer {
    public static void loadLibraries(){
        String os = System.getProperty("os.name");
        LibraryType libraryType;
        os = os.toLowerCase();
        if(os.contains("linux") || os.contains("unix") || os.contains("bsd") || os.contains("android")){
            libraryType = LibraryType.UNIX_LIBRARY;
        } else if(os.contains("apple") || os.contains("mac") || os.contains("ios") || os.contains("ipados")){
            libraryType = LibraryType.APPLE_LIBRARY;
        } else if(os.contains("windows") || os.contains("win32") || os.contains("win")){
            libraryType = LibraryType.WINDOWS_LIBRARY;
        } else{
            libraryType = null;
            throw new RuntimeException("Failed to detect os type!");
        }
        File file = new File("JavaModules");
        Arrays.stream(file.listFiles()).filter(element->element.getName().contains(libraryType.getExtension())).forEach(element-> System.load(element.getAbsolutePath()));
    }
}

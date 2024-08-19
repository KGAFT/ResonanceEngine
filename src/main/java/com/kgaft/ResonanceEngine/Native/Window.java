package com.kgaft.ResonanceEngine.Native;

public class Window {
    public static native long createWindow(int width, int height, String windowTitle);
    public Window(long windowHandle){
        this.windowHandle = windowHandle;
    }
    private long windowHandle;
    public native boolean needToClose();
    public native void preRenderEvents();
    public native void postRenderEvents();
    public native int getWidth();
    public native int getHeight();
    public native void setSize(int width, int height);
    public native void setTitle(String title);
    public native void addResizeCallback(IWindowResizeCallback resizeCallback);
    public native void removeResizeCallback(IWindowResizeCallback resizeCallback);
    public native void enableRefreshRateInfo();
    public native void disableRefreshRateInfo();
    public native void destroy();

}

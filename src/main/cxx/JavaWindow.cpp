#include "com_kgaft_ResonanceEngine_Native_Window.h"
#include "Window/Window.hxx"

JNIEXPORT jlong JNICALL Java_com_kgaft_ResonanceEngine_Native_Window_createWindow
  (JNIEnv *env, jclass, jint width, jint height, jstring title){
     const char *titleC = env->GetStringUTFChars(title, 0);
     auto window = Window::createWindow(width, height, titleC);
     env->ReleaseStringUTFChars(title, titleC);
     return (jlong)window;
  }
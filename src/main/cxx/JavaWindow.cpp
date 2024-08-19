#include "Utils/JniString.hpp"
#include "Window/Window.hxx"
#include "com_kgaft_ResonanceEngine_Native_Window.h"
#include <map>
#include "JavaWindow.h"


std::map<jobject, JResizeCallback *> resizeCallbacks;

JNIEXPORT jlong JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_createWindow(JNIEnv *env, jclass,
                                                          jint width,
                                                          jint height,
                                                          jstring title) {
  const char *titleC = env->GetStringUTFChars(title, 0);
  auto window = Window::createWindow(width, height, titleC);
  env->ReleaseStringUTFChars(title, titleC);
  return (jlong)window;
}
JNIEXPORT jboolean JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_needToClose(JNIEnv *env,
                                                         jobject windowObject) {
  auto window = GetWindow(windowObject);
  return window->needToClose();
}
JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_postRenderEvents(
    JNIEnv *env, jobject windowObject) {
  auto window = GetWindow(windowObject);
  window->postRenderEvents();
}

JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_preRenderEvents(
    JNIEnv *env, jobject windowObject) {
  GetWindow(windowObject)->preRenderEvents();
}

JNIEXPORT jint JNICALL Java_com_kgaft_ResonanceEngine_Native_Window_getWidth(
    JNIEnv *env, jobject windowObject) {
  return GetWindow(windowObject)->getWidth();
}

JNIEXPORT jint JNICALL Java_com_kgaft_ResonanceEngine_Native_Window_getHeight(
    JNIEnv *env, jobject windowObject) {
  return GetWindow(windowObject)->getHeight();
}

JNIEXPORT void JNICALL Java_com_kgaft_ResonanceEngine_Native_Window_setSize(
    JNIEnv *env, jobject windowObject, jint width, jint height) {
  return GetWindow(windowObject)->setSize(width, height);
}

JNIEXPORT void JNICALL Java_com_kgaft_ResonanceEngine_Native_Window_setTitle(
    JNIEnv *env, jobject windowObject, jstring windowTitle) {
  JNIString title(env, windowTitle);
  GetWindow(windowObject)->setTitle(title.c_str());
}

JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_addResizeCallback(
    JNIEnv *env, jobject windowObject, jobject resizeCallback) {
  auto resizeCB = new JResizeCallback(env, resizeCallback);
  resizeCallbacks[resizeCallback] = resizeCB;
  GetWindow(windowObject)->addResizeCallback(resizeCB);
}

JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_removeResizeCallback(
    JNIEnv *env, jobject windowObject, jobject resizeCallback) {
  auto cb = resizeCallbacks[resizeCallback];
  GetWindow(windowObject)->removeResizeCallback(cb);
  resizeCallbacks.erase(resizeCallback);
}

JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_enableRefreshRateInfo(
    JNIEnv *env, jobject windowObject) {

  GetWindow(windowObject)->enableRefreshRateInfo();
}

/*
 * Class:     com_kgaft_ResonanceEngine_Native_Window
 * Method:    disableRefreshRateInfo
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_disableRefreshRateInfo(
    JNIEnv *env, jobject windowObject) {
  GetWindow(windowObject)->disableRefreshInfo();
}

/*
 * Class:     com_kgaft_ResonanceEngine_Native_Window
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_destroy(JNIEnv *env, jobject windowObject){
  GetWindow(windowObject)->destroy();
}
#include "Utils/JniString.hpp"
#include "Window/Window.hxx"
#include "com_kgaft_ResonanceEngine_Native_Window.h"

#define GetWindow(windowObject)                                                \
  ((Window *)env->GetLongField(                                                \
      windowObject, env->GetFieldID(env->GetObjectClass(windowObject),         \
                                    "windowHandle", "J")))


class JResizeCallback : public IWindowResizeCallback{
public:
  JResizeCallback(JNIEnv* env, jobject callback) : env(env), callbackObject(callback), callbackId(env->GetMethodID(env->GetObjectClass(callback), "resized", "(I,I)V")){
  }
private:
  jmethodID callbackId;
  jobject callbackObject;
  JNIEnv* env;  
public:
  void resized(uint32_t width, uint32_t height) override {
    env->CallVoidMethodA(jobject obj, jmethodID methodID, const jvalue *args)
  }
};
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
      env.GetMet
    }

/*
 * Class:     com_kgaft_ResonanceEngine_Native_Window
 * Method:    removeResizeCallback
 * Signature: (Lcom/kgaft/ResonanceEngine/Native/IWindowResizeCallback;)V
 */
JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_removeResizeCallback(JNIEnv *,
                                                                  jobject,
                                                                  jobject);

/*
 * Class:     com_kgaft_ResonanceEngine_Native_Window
 * Method:    enableRefreshRateInfo
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_enableRefreshRateInfo(JNIEnv *,
                                                                   jobject);

/*
 * Class:     com_kgaft_ResonanceEngine_Native_Window
 * Method:    disableRefreshRateInfo
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_disableRefreshRateInfo(JNIEnv *,
                                                                    jobject);

/*
 * Class:     com_kgaft_ResonanceEngine_Native_Window
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_com_kgaft_ResonanceEngine_Native_Window_destroy(JNIEnv *, jobject);
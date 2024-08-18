#ifndef MENGINE_SDLWINDOW_HPP
#define MENGINE_SDLWINDOW_HPP

#include <SDL3/SDL.h>
#define WINDOW_KEY(x) SDL_SCANCODE_##x


#include "VulkanLib/MemoryUtils/IDestroyableObject.hpp"
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "VulkanLib/Device/Synchronization/IResizeCallback.hpp"


enum WindowInputMode {
    MODE_FREE_CURSOR = 0,
    MODE_RETURN_INTO_CENTER = 1,
    MODE_BOTH_MODES = 2
};

class IWindowMouseMovementCallback {
public:
    virtual void moved(glm::vec2 oldPos, glm::vec2 newPos) = 0;

    virtual WindowInputMode getRequireWorkMode() = 0;
};

enum KeyAction {
    KEY_PRESSED = 0,
    KEY_CLICKED = 1,
    KEY_HOLD = 2,
    KEY_RELEASED = 3
};

struct WindowKey {
    uint32_t scanCode;
    KeyAction action;
    bool isMouseKey;
};

struct InternalKey {
    uint32_t scanCode;
    uint8_t status;
    time_t lastActiveTime;
    bool isMouse;
};

class IWindowKeyCallback {
public:
    virtual void keyPressed(WindowKey *key) = 0;

    /**
                  @param output array output
                * @return amount of keys
                */
    virtual unsigned int getKeys(WindowKey **output) = 0;

    virtual size_t getHoldDelay() = 0;
};

class Window;

class WindowInputSystem {
    friend class Window;

public:
    explicit WindowInputSystem(Window *window);

private:
    std::vector<IWindowMouseMovementCallback *> mouseCallbacks;
    std::vector<IWindowKeyCallback *> keyCallbacks;
    std::vector<InternalKey *> keys;
    glm::vec2 oldMousePosition;
    glm::vec2 curMousePos;
    Window *window;
    WindowInputMode currentMode = MODE_FREE_CURSOR;

public:
    void registerKeyCallback(IWindowKeyCallback *callback);

    void registerMouseCallback(IWindowMouseMovementCallback *callback);

    void removeKeyCallback(IWindowKeyCallback *callback);

    void removeMouseCallback(IWindowMouseMovementCallback *callback);

    void setMode(WindowInputMode mode);

    WindowInputMode getMode();

    ~WindowInputSystem();


private:
    void checkKeyCallback();

    void checkMovementCallbacks();

    void processKeyCallback(IWindowKeyCallback *callback);

    bool isKeyExist(int scanCode);

    void getKeyFromArray(uint32_t scanCode, InternalKey **pOutput);
};

class IWindowResizeCallback {
public:
    virtual void resized(uint32_t width, uint32_t height) = 0;
};

class IWindowOnResizeProcessStarted {
public:
    virtual void resizing(uint32_t currentWidht, uint32_t currentHeight) = 0;
};

class Window : public IDestroyableObject {
public:
    static Window *createWindow(uint32_t width, uint32_t height,
                                const char *title);
    static void getRequiredExtensions(std::vector<const char*>& extOutput);
private:
    Window(void *windowHandle, uint32_t width, uint32_t height,
           const char *title): windowHandle(windowHandle), width(width), height(height),
                               title(title), inputSystem(this) {
    }

    void *windowHandle;
    uint32_t width;
    uint32_t height;
    const char *title;
    std::vector<IResizeCallback *> resizeCallbacks;
    std::vector<IWindowOnResizeProcessStarted *> continuousResizeEvents;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    WindowInputSystem inputSystem;
    long long counter = 0;
    bool refreshInfo = false;
    double prevTime = 0;
    bool isNeedToClose = false;

public:
    bool needToClose();

    VkSurfaceKHR getWindowSurface(vk::Instance instance);

    void *getWindowHandle();

    uint32_t getWidth() const;

    uint32_t getHeight() const;

    void setSize(uint32_t width, uint32_t height);

    void setTitle(const char *title);

    const char *getTitle();

    void addContinuousResizeCallback(IWindowOnResizeProcessStarted *callback);

    void addResizeCallback(IResizeCallback *resizeCallback);

    void removeResizeCallback(IResizeCallback *resizeCallback);

    void preRenderEvents();

    void postRenderEvents();

    WindowInputSystem &getInputSystem();

    void enableRefreshRateInfo();

    void disableRefreshInfo();

    void continuousResizeEvent(uint32_t width, uint32_t height);

    void destroy() override;

private:
    void sizeCallbacks();

    void calculateRefreshData();

    static void pollEvents();
};


#endif
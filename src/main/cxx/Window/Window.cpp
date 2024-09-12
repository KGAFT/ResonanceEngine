
#ifdef SDLWindowUsed
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <iostream>
#include <stdexcept>
#include "Window.hxx"

bool sdlInitialized = false;

std::vector<Window *> windowInstances = std::vector<Window *>();

void Window::calculateRefreshData() {
    double crntTime = SDL_GetTicks() / 1000;
    double timeDiff = crntTime - prevTime;
    counter++;

    if (timeDiff >= 1.0 / 30.0) {
        std::string FPS = std::to_string((int) ((1.0 / timeDiff) * counter));
        std::string ms = std::to_string((timeDiff / counter) * 1000);
        SDL_SetWindowTitle((SDL_Window *) windowHandle, (std::string(title) + " FPS: " + FPS + " ms: " + ms).c_str());

        prevTime = crntTime;
        counter = 0;
    }
}

void Window::getRequiredExtensions(std::vector<const char*>& extOutput) {
    uint32_t count = 0;
    auto exts = SDL_Vulkan_GetInstanceExtensions(&count);

    for(uint32_t i = 0 ; i < count; i++) {
        extOutput.push_back(exts[i]);
    }
}

void Window::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        for (const auto &item: windowInstances) {
            if (SDL_GetWindowID((SDL_Window *) item->windowHandle) ==
                event.window.windowID) {
                switch (event.type) {
                    case SDL_EVENT_WINDOW_RESIZED:
                        item->setSize(event.window.data1, event.window.data2);
                        break;
                    case SDL_EVENT_MOUSE_MOTION:
                        item->inputSystem.curMousePos.x = event.motion.x;
                        item->inputSystem.curMousePos.y = event.motion.y;
                        break;
                    case SDL_EVENT_QUIT:
                        item->isNeedToClose = true;
                        break;
                }
                break;
            }
        }
        if (event.type == SDL_EVENT_QUIT) {
            std::for_each(windowInstances.begin(), windowInstances.end(),
                          [&](auto &item) { item->isNeedToClose = true; });
        }
    }
}

bool resizingEventWatcher(void *data, SDL_Event *event) {
    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        SDL_Window *win = SDL_GetWindowFromID(event->window.windowID);
        for (auto &item: windowInstances) {
            if (win == item->getWindowHandle()) {
                item->continuousResizeEvent(event->window.data1, event->window.data2);
                break;
            }
        }
    }
    return 0;
}

Window *Window::createWindow(uint32_t width, uint32_t height,
                             const char *title) {
    if (!sdlInitialized) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        sdlInitialized = true;
    }
    SDL_Window *window = SDL_CreateWindow(
        title, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        auto error = SDL_GetError();
        std::cerr << error  << std::endl;
        throw std::runtime_error("Failed to create sdl window");
    }
    SDL_AddEventWatch(resizingEventWatcher, window);
    auto result = new Window(window, width, height, title);
    windowInstances.push_back(result);
    return result;
}


bool Window::needToClose() { return isNeedToClose; }

VkSurfaceKHR Window::getWindowSurface(VkInstance instance) {
    if (surface==0) {
        SDL_Vulkan_CreateSurface((SDL_Window *) windowHandle, instance, nullptr,
                                      &surface);
    }
    if(surface==0){
        auto error = SDL_GetError();
        std::cerr << windowHandle <<" "<< error  << std::endl;
        throw std::runtime_error("Failed to acquire surface");
    }
    return surface;
}

void *Window::getWindowHandle() { return windowHandle; }

// void setMonitor(Monitor *monitor);

uint32_t Window::getWidth() const { return width; }

uint32_t Window::getHeight() const { return height; }

void Window::setSize(uint32_t width, uint32_t height) {
    SDL_SetWindowSize((SDL_Window *) windowHandle, width, height);
    std::for_each(resizeCallbacks.begin(), resizeCallbacks.end(),
                  [&](auto &item) { item->resized(width, height); });
    SDL_UpdateWindowSurface((SDL_Window *) windowHandle);
    this->width = width;
    this->height = height;
}

void Window::setTitle(const char *title) {
    SDL_SetWindowTitle((SDL_Window *) windowHandle, title);
    this->title = title;
}

const char *Window::getTitle() { return title; }

void Window::addResizeCallback(IResizeCallback *resizeCallback) {
    resizeCallbacks.push_back(resizeCallback);
}

void Window::addContinuousResizeCallback(
    IWindowOnResizeProcessStarted *callback) {
    continuousResizeEvents.push_back(callback);
}

void Window::removeResizeCallback(IResizeCallback *resizeCallback) {
    uint32_t removeIndex = -1;
    for (uint32_t i = 0; i < resizeCallbacks.size(); i++) {
        if (resizeCallbacks[i] == resizeCallback) {
            removeIndex = i;
        }
    }

    if (removeIndex != -1) {
        resizeCallbacks.erase(resizeCallbacks.begin() + removeIndex);
    }
}

// Monitor *getAttachMonitor() const;

void Window::preRenderEvents() {
    inputSystem.checkMovementCallbacks();
    inputSystem.checkKeyCallback();
}

void Window::postRenderEvents() {
    Window::pollEvents();
    if (refreshInfo) {
        calculateRefreshData();
    }
}

void Window::continuousResizeEvent(uint32_t width, uint32_t height) {
    for (auto &citem: continuousResizeEvents) {
        citem->resizing(width, height);
    }
}


WindowInputSystem &Window::getInputSystem() { return inputSystem; }

void Window::enableRefreshRateInfo() { refreshInfo = true; }

void Window::disableRefreshInfo() { refreshInfo = false; }

WindowInputSystem::WindowInputSystem(Window *window) : oldMousePosition(0), curMousePos(0), window(window) {
}

void WindowInputSystem::registerKeyCallback(IWindowKeyCallback *callback) {
    unsigned int keyCount = 0;
    WindowKey *cbKeys;
    keyCount = callback->getKeys(&cbKeys);
    time_t curTime;
    time(&curTime);
    for (unsigned int i = 0; i < keyCount; i++) {
        if (!isKeyExist(cbKeys[i].scanCode)) {
            auto *key = new InternalKey;
            key->lastActiveTime = curTime;
            key->status = 0;
            key->scanCode = cbKeys[i].scanCode;
            key->isMouse = cbKeys[i].isMouseKey;
            this->keys.push_back(key);
        }
    }
    this->keyCallbacks.push_back(callback);
}

void WindowInputSystem::registerMouseCallback(
    IWindowMouseMovementCallback *callback) {
    mouseCallbacks.push_back(callback);
}

void WindowInputSystem::setMode(WindowInputMode mode) {
    currentMode = mode;
    switch (currentMode) {
        case WindowInputMode::MODE_FREE_CURSOR:
            SDL_ShowCursor();
            break;
        case WindowInputMode::MODE_RETURN_INTO_CENTER:
            SDL_HideCursor();
            break;
        default:
            break;
    }
}

WindowInputSystem::~WindowInputSystem() {
    for (auto &el: keys) {
        delete el;
    }
}

void WindowInputSystem::checkKeyCallback() {
    for (IWindowKeyCallback *el: keyCallbacks) {
        processKeyCallback(el);
    }
}

void WindowInputSystem::checkMovementCallbacks() {
    if (curMousePos != oldMousePosition) {
        for (auto &el: mouseCallbacks) {
            if (el->getRequireWorkMode() == MODE_BOTH_MODES ||
                el->getRequireWorkMode() == currentMode) {
                el->moved(oldMousePosition, curMousePos);
            }
        }
    }
    if (currentMode == MODE_RETURN_INTO_CENTER) {
        int width = window->getWidth();
        int height = window->getHeight();
        if ((curMousePos.x >= width - 10 || curMousePos.y >= height - 10) ||
            (curMousePos.x <= 0 || curMousePos.y <= 0)) {
            SDL_WarpMouseInWindow((SDL_Window *) window->getWindowHandle(), width / 2, height / 2);
            curMousePos = glm::vec2(width / 2, height / 2);
        }
    }
    oldMousePosition = curMousePos;
}

void WindowInputSystem::processKeyCallback(IWindowKeyCallback *callback) {
    WindowKey *cbKeys;
    uint32_t keyCount = callback->getKeys(&cbKeys);

    float x, y;
    auto sdlKeys = SDL_GetKeyboardState(nullptr);
    auto mouseKeys = SDL_GetMouseState(&x, &y);
    InternalKey *key;
    SDL_PumpEvents();
    for (unsigned int i = 0; i < keyCount; i++) {
        getKeyFromArray(cbKeys[i].scanCode, &key);
        Uint8 curStatus =
                key->isMouse ? mouseKeys & key->scanCode : sdlKeys[key->scanCode];
        time_t curTime;
        time(&curTime);
        switch (cbKeys[i].action) {
            case KEY_PRESSED:
                if (curStatus) {
                    callback->keyPressed(&cbKeys[i]);
                }
                break;
            case KEY_CLICKED:
                if (!key->status && curStatus) {
                    callback->keyPressed(&cbKeys[i]);
                }
                break;
            case KEY_HOLD:
                if (key->status && curStatus &&
                    ((time_t) callback->getHoldDelay()) <=
                    (curTime - key->lastActiveTime) * 1000) {
                    callback->keyPressed(&cbKeys[i]);
                }
                key->lastActiveTime = curTime;
                break;
            case KEY_RELEASED:
                if (key->status && !curStatus) {
                    callback->keyPressed(&cbKeys[i]);
                }
                break;
        }
        key->status = curStatus;
        key->lastActiveTime =
                cbKeys[i].action == KEY_HOLD ? key->lastActiveTime : curTime;
        SDL_PumpEvents();
    }
}

WindowInputMode WindowInputSystem::getMode() { return currentMode; }

bool WindowInputSystem::isKeyExist(int scanCode) {
    for (auto el: keys) {
        if (el->scanCode == scanCode) {
            return true;
        }
    }
    return false;
}

void WindowInputSystem::getKeyFromArray(uint32_t scanCode,
                                        InternalKey **pOutput) {
    for (auto el: keys) {
        if (el->scanCode == scanCode) {
            *pOutput = el;
            break;
        }
    }
}

void WindowInputSystem::removeKeyCallback(IWindowKeyCallback *callback) {
    int i = -1;
    for (unsigned int g = 0; g < keyCallbacks.size(); i++) {
        if (keyCallbacks[g] == callback) {
            i = g;
            break;
        }
    }
    if (i != -1) {
        keyCallbacks.erase(keyCallbacks.begin() + i);
    }
}

void WindowInputSystem::removeMouseCallback(
    IWindowMouseMovementCallback *callback) {
    int i = -1;
    for (unsigned int g = 0; g < mouseCallbacks.size(); i++) {
        if (mouseCallbacks[g] == callback) {
            i = g;
            break;
        }
    }
    if (i != -1) {
        mouseCallbacks.erase(mouseCallbacks.begin() + i);
    }
}

void Window::destroy() {
    destroyed = true;
    SDL_DestroyWindow((SDL_Window *) windowHandle);
}

void Window::sizeCallbacks() {
    for (auto &el: resizeCallbacks) {
        el->resized(width, height);
    }
}
#endif

#ifdef GLFWWindowUsed
#include <iostream>
#include <vulkan/vulkan.hpp>
#include "Window.hxx"

bool glfwInitial = false;

std::vector<Window *> windowInstances = std::vector<Window *>();

void resized(GLFWwindow *window, int width, int height) {
    for (auto &el: windowInstances) {
        if (el->getWindowHandle() == window) {
            el->setSize(width, height);
        }
    }
}

void Window::getRequiredExtensions(std::vector<const char*>& extOutput) {
    if(!glfwInitial) {
        glfwInit();
        glfwInitial = true;
    }
    uint32_t count = 0;
    auto exts = glfwGetRequiredInstanceExtensions(&count);

    for(uint32_t i = 0 ; i < count; i++) {
        extOutput.push_back(exts[i]);
    }
}

Window *Window::createWindow(uint32_t width, uint32_t height, const char *title
) {
    if(!glfwInitial) {
        glfwInit();
        glfwInitial = true;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window) {
        Window *res = new Window(
            window, width, height, title);
        glfwSetWindowSizeCallback(window, resized);


        windowInstances.push_back(res);
        return res;
    }
    const char* desc = nullptr;
    glfwGetError(&desc);
    std::cerr<<desc<<std::endl;
    throw std::runtime_error("Failed to create window");
}


WindowInputSystem &Window::getInputSystem() { return inputSystem; }


void Window::preRenderEvents() {
    inputSystem.checkMovementCallbacks();
    inputSystem.checkKeyCallback();
}

void Window::postRenderEvents() {

    pollEvents();
    if (refreshInfo) {
        calculateRefreshData();
    }
}

void Window::sizeCallbacks() {
    for (auto &el: resizeCallbacks) {
        el->resized(width, height);
    }
}

bool Window::needToClose() { return glfwWindowShouldClose((GLFWwindow*)windowHandle); }

VkSurfaceKHR Window::getWindowSurface(vk::Instance instance) {
    if (surface == VK_NULL_HANDLE) {
        glfwCreateWindowSurface(instance, (GLFWwindow*)windowHandle, nullptr, &surface);
        if(!surface) {
            const char* desc = nullptr;
            glfwGetError(&desc);
            std::cerr<<desc<<std::endl;
            throw std::runtime_error("Failed to create surface");
        }
    }
    return surface;
}

void *Window::getWindowHandle() { return windowHandle; }



uint32_t Window::getWidth() const { return width; }

uint32_t Window::getHeight() const { return height; }

void Window::setSize(uint32_t width, uint32_t height) {

    this->width = width;
    this->height = height;
    glfwSetWindowSize((GLFWwindow*)windowHandle, width, height);
}

void Window::setTitle(const char *title) {
    this->title = title;
    glfwSetWindowTitle((GLFWwindow*)windowHandle, title);
}

const char *Window::getTitle() { return title; }

void Window::addResizeCallback(IResizeCallback *resizeCallback) {
    resizeCallbacks.push_back(resizeCallback);
}

void Window::removeResizeCallback(IResizeCallback *resizeCallback) {
    int removeIndex = -1;
    for (uint32_t i = 0; i < resizeCallbacks.size(); i++) {
        if (resizeCallbacks[i] == resizeCallback) {
            removeIndex = i;
        }
    }

    if (removeIndex != -1) {
        resizeCallbacks.erase(resizeCallbacks.begin() + removeIndex);
    }
}

void Window::enableRefreshRateInfo() {
    refreshInfo = true;
}

void Window::calculateRefreshData() {
    double crntTime = glfwGetTime();
    double timeDiff = crntTime - prevTime;
    counter++;

    if (timeDiff >= 1.0 / 30.0) {
        std::string FPS = std::to_string((int) ((1.0 / timeDiff) * counter));
        std::string ms = std::to_string((timeDiff / counter) * 1000);
        glfwSetWindowTitle((GLFWwindow*)windowHandle, (std::string(title) + " FPS: " + FPS + " ms: " + ms).c_str());

        prevTime = crntTime;
        counter = 0;
    }
}

void Window::disableRefreshInfo() {
    refreshInfo = false;
}

void Window::destroy() {
    destroyed = true;
    glfwDestroyWindow((GLFWwindow*)windowHandle);
}
void Window::addContinuousResizeCallback(
    IWindowOnResizeProcessStarted *callback) {
}
void Window::continuousResizeEvent(uint32_t width, uint32_t height) {
}
void Window::pollEvents() {
    glfwPollEvents();
}

WindowInputSystem::WindowInputSystem(Window *window) : oldMousePosition(0), curMousePos(0), window(window) {
}


void WindowInputSystem::registerKeyCallback(IWindowKeyCallback *callback) {
    unsigned int keyCount = 0;
    WindowKey *cbKeys;
    keyCount = callback->getKeys(&cbKeys);
    time_t curTime;
    time(&curTime);
    for (unsigned int i = 0; i < keyCount; i++) {
        if (!isKeyExist(cbKeys[i].scanCode)) {
            InternalKey *key = new InternalKey;
            key->lastActiveTime = curTime;
            key->status = GLFW_RELEASE;
            key->scanCode = cbKeys[i].scanCode;
            key->isMouse =
                    cbKeys[i].scanCode >= 0 && cbKeys[i].scanCode <= 7;
            this->keys.push_back(key);
        }
    }
    this->keyCallbacks.push_back(callback);
}

void WindowInputSystem::registerMouseCallback(
        IWindowMouseMovementCallback *callback) {
    mouseCallbacks.push_back(callback);
}

void WindowInputSystem::setMode(WindowInputMode mode) {
    currentMode = mode;
    switch(currentMode){
        case WindowInputMode::MODE_FREE_CURSOR:
            glfwSetInputMode((GLFWwindow*)window->getWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case WindowInputMode::MODE_RETURN_INTO_CENTER:
            glfwSetInputMode((GLFWwindow*)window->getWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        default:
            break;
    }
}

WindowInputSystem::~WindowInputSystem() {
    for (auto &el: keys) {
        delete el;
    }
}

void WindowInputSystem::checkKeyCallback() {

    for (IWindowKeyCallback *el: keyCallbacks) {
        processKeyCallback(el);
    }
}

void WindowInputSystem::checkMovementCallbacks() {
    double mousePosX, mousePosY = 0;
    glfwGetCursorPos((GLFWwindow*)window->getWindowHandle(), &mousePosX, &mousePosY);
    glm::vec2 curMousePos(mousePosX, mousePosY);
    if (curMousePos != oldMousePosition) {
        for (auto &el: mouseCallbacks) {
            if (el->getRequireWorkMode() == MODE_BOTH_MODES ||
                el->getRequireWorkMode() == currentMode) {
                el->moved(oldMousePosition, curMousePos);
            }
        }
    }
    if (currentMode == MODE_RETURN_INTO_CENTER) {
        int width, height = 0;
        glfwGetWindowSize((GLFWwindow*)window->getWindowHandle(), &width, &height);
        if((mousePosX>=width-30 || mousePosY>=height-30) || (mousePosX<=30 || mousePosY<=30)){
            glfwSetCursorPos((GLFWwindow*)window->getWindowHandle(), width / 2, height / 2);
            curMousePos = glm::vec2(width / 2, height / 2);
        }
    }
    oldMousePosition = curMousePos;
}

void WindowInputSystem::processKeyCallback(IWindowKeyCallback *callback) {
    unsigned int keyCount = 0;
    WindowKey *cbKeys;
    keyCount = callback->getKeys(&cbKeys);
    InternalKey *key;
    for (unsigned int i = 0; i < keyCount; i++) {
        getKeyFromArray(cbKeys[i].scanCode, &key);
        int curStatus = key->isMouse
                        ? glfwGetMouseButton((GLFWwindow*)window->getWindowHandle(), key->scanCode)
                        : glfwGetKey((GLFWwindow*)window->getWindowHandle(), key->scanCode);
        time_t curTime;
        time(&curTime);
        switch (cbKeys[i].action) {
            case KEY_PRESSED:
                if (curStatus == GLFW_PRESS) {
                    callback->keyPressed(&cbKeys[i]);
                }
                break;
            case KEY_CLICKED:
                if (key->status == GLFW_RELEASE && curStatus == GLFW_PRESS) {
                    callback->keyPressed(&cbKeys[i]);
                }
                break;
            case KEY_HOLD:
                if (key->status == GLFW_PRESS && curStatus == GLFW_PRESS &&
                    ((time_t) callback->getHoldDelay()) <= (curTime - key->lastActiveTime) * 1000) {
                    callback->keyPressed(&cbKeys[i]);
                }
                key->lastActiveTime = curTime;
                break;
            case KEY_RELEASED:
                if (key->status == GLFW_PRESS && curStatus == GLFW_RELEASE) {
                    callback->keyPressed(&cbKeys[i]);
                }
                break;
        }
        key->status = curStatus;
        key->lastActiveTime =
                cbKeys[i].action == KEY_HOLD ? key->lastActiveTime : curTime;
    }
}

WindowInputMode WindowInputSystem::getMode() { return currentMode; }

bool WindowInputSystem::isKeyExist(int scanCode) {
    for (auto el: keys) {
        if (el->scanCode == scanCode) {
            return true;
        }
    }
    return false;
}

void WindowInputSystem::getKeyFromArray(uint32_t scanCode, InternalKey **pOutput) {
    for (auto el: keys) {
        if (el->scanCode == scanCode) {
            *pOutput = el;
            break;
        }
    }
}

void WindowInputSystem::removeKeyCallback(IWindowKeyCallback *callback) {
    int i = -1;
    for (unsigned int g = 0; g < keyCallbacks.size(); i++) {
        if (keyCallbacks[g] == callback) {
            i = g;
            break;
        }
    }
    if (i != -1) {
        keyCallbacks.erase(keyCallbacks.begin() + i);
    }
}

void WindowInputSystem::removeMouseCallback(
        IWindowMouseMovementCallback *callback) {
    int i = -1;
    for (unsigned int g = 0; g < mouseCallbacks.size(); i++) {
        if (mouseCallbacks[g] == callback) {
            i = g;
            break;
        }
    }
    if (i != -1) {
        mouseCallbacks.erase(mouseCallbacks.begin() + i);
    }
}

#endif
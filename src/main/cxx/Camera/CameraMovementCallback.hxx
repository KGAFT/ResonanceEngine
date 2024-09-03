#pragma once


#include "Camera.hxx"
#include "Window/Window.hxx"

class CameraMovementCallback : public IWindowKeyCallback {
public:
    CameraMovementCallback(Window* window, Camera* camera);
private:
    std::vector<WindowKey> keys;
    Camera* camera = nullptr;
    glm::vec3 sensitivity;
    Window* window = nullptr;
private:
    void loadKeys();
public:
  void setSensitivity(glm::vec3 sensitivity);
  void setCamera(Camera* camera);
  void keyPressed(WindowKey *key) override;
  unsigned int getKeys(WindowKey **output) override;
  size_t getHoldDelay() override;

    virtual ~CameraMovementCallback();
};

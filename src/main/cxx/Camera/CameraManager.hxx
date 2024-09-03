#pragma once


#include "Camera.hxx"
#include "CameraMovementCallback.hxx"
#include "CameraRotationCallback.hxx"
#include <stdexcept>

#include "Window/Window.hxx"


class CameraManager {
public:
  CameraManager(Window* window);
private:
  Window* window;
  Camera *camera;
  CameraMovementCallback* movementCallback;
  CameraRotationCallback* rotationCallback;
public:
  void setCamera(Camera* camera);
  void setMovementSensitivity(glm::vec3 sensitivity);
  void setRotationSensitivity(glm::vec2 sensitivity);
  Camera* getCurrentCamera();
  ~CameraManager();
};

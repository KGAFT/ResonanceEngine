#pragma once


#include "Camera.hxx"
#include "Window/Window.hxx"


class CameraRotationCallback : public IWindowMouseMovementCallback {
public:
    CameraRotationCallback(Camera* camera);
private:
    Camera* camera;
    glm::vec2 sensitivity = glm::vec2(40, 40);
public:
  void setCamera(Camera* pCamera);
  void setSensitivity(glm::vec2 newSensitivity);
  void moved(glm::vec2 oldPos, glm::vec2 newPos) override;
  WindowInputMode getRequireWorkMode() override;

    virtual ~CameraRotationCallback();
};

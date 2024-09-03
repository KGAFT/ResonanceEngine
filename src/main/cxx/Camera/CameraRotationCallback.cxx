#include "CameraRotationCallback.hxx"

CameraRotationCallback::CameraRotationCallback(Camera *camera)
        : camera(camera) {}

void CameraRotationCallback::setCamera(Camera *pCamera) {
    CameraRotationCallback::camera = pCamera;
}

void CameraRotationCallback::moved(glm::vec2 oldPos, glm::vec2 newPos) {
    float rotX = sensitivity.x * (newPos.x - oldPos.x) / oldPos.x / 2;
    float rotY = sensitivity.y * (newPos.y - oldPos.y) / oldPos.y / 2;
    camera->rotateCam(rotX, rotY);
}

WindowInputMode CameraRotationCallback::getRequireWorkMode() {
    return MODE_RETURN_INTO_CENTER;
}

void CameraRotationCallback::setSensitivity(glm::vec2 newSensitivity) {
    CameraRotationCallback::sensitivity = newSensitivity;
}

CameraRotationCallback::~CameraRotationCallback() {

}


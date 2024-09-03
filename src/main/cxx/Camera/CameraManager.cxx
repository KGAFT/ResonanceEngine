#include "CameraManager.hxx"


CameraManager::CameraManager(Window *window) {
    if (window == nullptr) {
        throw std::runtime_error("Error window cannot be null");
    }
    this->window = window;
    this->camera = new Camera(glm::vec3(0, 0, 2.0f));

    this->rotationCallback = new CameraRotationCallback(camera);
    this->movementCallback = new CameraMovementCallback(window, camera);

    window->getInputSystem().registerKeyCallback(movementCallback);
    window->getInputSystem().registerMouseCallback(rotationCallback);
}

void CameraManager::setCamera(Camera *camera) {
    this->camera = camera;
    this->rotationCallback->setCamera(camera);
    this->movementCallback->setCamera(camera);
}

Camera *CameraManager::getCurrentCamera() {
    return this->camera;
}

CameraManager::~CameraManager() {
    window->getInputSystem().removeKeyCallback(movementCallback);
    window->getInputSystem().removeMouseCallback(rotationCallback);
    delete rotationCallback;
    delete movementCallback;

}

void CameraManager::setMovementSensitivity(glm::vec3 sensitivity) {
    movementCallback->setSensitivity(sensitivity);
}

void CameraManager::setRotationSensitivity(glm::vec2 sensitivity) {
    rotationCallback->setSensitivity(sensitivity);
}

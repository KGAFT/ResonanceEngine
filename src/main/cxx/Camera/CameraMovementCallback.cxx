#include <iostream>
#include "CameraMovementCallback.hxx"

#include <Window/Window.hxx>

CameraMovementCallback::CameraMovementCallback(Window *window,
                                               Camera *camera)
    : camera(camera), window(window) {
        loadKeys();
    }

void CameraMovementCallback::keyPressed(WindowKey *key){
    float leftRight = 0;
    float forwardBackward = 0;
    float upDown = 0;
    switch (key->scanCode) {
        case WINDOW_KEY(F1):
            window->getInputSystem().setMode((WindowInputMode)!window->getInputSystem().getMode());
            break;
        case WINDOW_KEY(W) :
            forwardBackward+=sensitivity.x;
            break;
        case WINDOW_KEY(S):
            forwardBackward-=sensitivity.x;
            break;

        case WINDOW_KEY(A) :
            leftRight-=sensitivity.z;
            break;
        case WINDOW_KEY(D):
            leftRight+=sensitivity.z;
            break;

        case WINDOW_KEY(SPACE):
            upDown+=sensitivity.y;
            break;
        case WINDOW_KEY(C):
            upDown-=sensitivity.y;
            break;
        default:
            break;
    }
    camera->moveCam(forwardBackward, leftRight, upDown);
}

void CameraMovementCallback::setSensitivity(glm::vec3 sensitivity){
    CameraMovementCallback::sensitivity = sensitivity;
}
void CameraMovementCallback::setCamera(Camera* camera){
    CameraMovementCallback::camera = camera;
}
unsigned int CameraMovementCallback::getKeys(WindowKey **output) {
  *output = keys.data();
  return (unsigned int) keys.size();
}

void CameraMovementCallback::loadKeys() {
  keys.resize(7);
  keys[0] = {WINDOW_KEY(F1), KEY_CLICKED, false};
  keys[1] = {WINDOW_KEY(W), KEY_PRESSED, false};
  keys[2] = {WINDOW_KEY(S), KEY_PRESSED, false};
  keys[3] = {WINDOW_KEY(A), KEY_PRESSED, false};
  keys[4] = {WINDOW_KEY(D), KEY_PRESSED, false};
  keys[5] = {WINDOW_KEY(SPACE), KEY_PRESSED, false};
  keys[6] = {WINDOW_KEY(C), KEY_PRESSED, false};
}

size_t CameraMovementCallback::getHoldDelay() { return 0; }

CameraMovementCallback::~CameraMovementCallback() {

}

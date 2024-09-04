//
// Created by kgaft on 9/3/2024.
//

#ifndef CAMERA_HXX
#define CAMERA_HXX

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

class Camera {
public:
    Camera(glm::vec3 position)
        : position(position) {}

private:
    glm::vec3 position;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);

public:
    glm::mat4 calculateCameraMatrix(float fov,
                                   float nearPlane, float farPlane, float aspectRatio) {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::lookAt(position, position + orientation, up);
        projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane,
                                      farPlane);
        return projection*view;
    }

    void calculateRayTracingCameraMatrix(glm::mat4 &viewOutput, glm::mat4 &projectionOutput, float fov, float nearPlane,
                                             float farPlane, float aspectRatio) {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::lookAt(position, position + orientation, up);
        projection =  glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
        projection[1][1] *= -1;
        projection = glm::inverse(projection);
        view = glm::inverse(view);
        viewOutput = view;
        projectionOutput = projection;
    }

    void rotateCam(float x, float y) {
        glm::vec3 newOrientation =
                glm::rotate(orientation, glm::radians(y * -1),
                            glm::normalize(glm::cross(orientation, up)));
        if (std::abs(glm::angle(newOrientation, up)) <= 90.0f) {
            orientation = newOrientation;
        }
        orientation = glm::rotate(orientation, glm::radians(-1*x), up);
    }

    void moveCam(float forwardBackward, float leftRight, float upDown) {
        position += forwardBackward * orientation;
        position += leftRight * glm::normalize(glm::cross(orientation, up));
        position += upDown * up;
    }

    void getPosition(glm::vec3 &output) {
        output = position;
    }

};


#endif //CAMERA_HPP

#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Position(position),
      Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      Up(up),
      WorldUp(up),
      Yaw(yaw),
      Pitch(pitch)
{
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Movement direction, float deltaTime)
{
    const float velocity = MovementSpeed * deltaTime;
    if (direction == Movement::Forward) {
        Position += Front * velocity;
    } else if (direction == Movement::Backward) {
        Position -= Front * velocity;
    } else if (direction == Movement::Left) {
        Position -= Right * velocity;
    } else if (direction == Movement::Right) {
        Position += Right * velocity;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch) {
        Pitch = std::clamp(Pitch, -89.0f, 89.0f);
    }

    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    // Convert angles to radians for trig
    const float yawRad = glm::radians(Yaw);
    const float pitchRad = glm::radians(Pitch);

    glm::vec3 front;
    front.x = cos(yawRad) * cos(pitchRad);
    front.y = sin(pitchRad);
    front.z = sin(yawRad) * cos(pitchRad);
    Front = glm::normalize(front);

    // Recalculate Right and Up vectors
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up    = glm::normalize(glm::cross(Right, Front));
}
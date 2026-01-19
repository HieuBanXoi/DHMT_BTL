#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Simple FPS-style camera.
// - WASD movement handled by ProcessKeyboard.
// - Mouse look handled by ProcessMouseMovement.
// - View matrix from glm::lookAt via GetViewMatrix().
class Camera
{
public:
    // Movement directions for ProcessKeyboard
    enum class Movement {
        Forward,
        Backward,
        Left,
        Right
    };

    // Constructors
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f
    );

    // Returns the view matrix calculated using glm::lookAt
    glm::mat4 GetViewMatrix() const;

    // Processes input received from any keyboard-like input system.
    // deltaTime scales movement to be frame-rate independent.
    void ProcessKeyboard(Movement direction, float deltaTime);

    // Processes input received from a mouse input system.
    // xoffset and yoffset are in screen space (pixels); sensitivity scales them.
    // If constrainPitch is true, pitch will be clamped to [-89, +89] degrees.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Accessors
    glm::vec3 GetPosition() const { return Position; }
    glm::vec3 GetFront() const { return Front; }
    glm::vec3 GetUp() const { return Up; }

    // Configurable parameters
    float MovementSpeed = 2.5f;
    float MouseSensitivity = 0.1f;

private:
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler angles (in degrees)
    float Yaw;
    float Pitch;

    // Recalculate Front, Right and Up from the current Euler angles.
    void UpdateCameraVectors();
};
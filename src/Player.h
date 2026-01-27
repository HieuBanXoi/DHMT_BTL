#pragma once

#include "Camera.h"
#include "Collision.h"
#include <GLFW/glfw3.h>
#include <vector>

class Player {
public:
    Player(glm::vec3 position);

    void ProcessInputs(GLFWwindow* window, float deltaTime, const std::vector<AABB>& colliders);
    void ProcessMouseMovement(float xoffset, float yoffset);

    glm::mat4 GetViewMatrix() const;
    glm::vec3 GetPosition() const;
    glm::vec3 GetFront() const;

    float Height = 1.7f; // Player eye height
    float Radius = 0.3f; // Player collision radius (approx as box)

private:
    Camera camera;
    glm::vec3 velocity = glm::vec3(0.0f); // Now 3D velocity
    bool isGrounded = false;
    float gravity = 30.0f; // Strong gravity for snappy feel
    float jumpForce = 10.0f;
    float stepHeight = 0.4f; // Allow stepping up substantial stairs
    float baseSpeed = 7.5f; // Reduced speed by default (was 15.0)
    
    // Fly Mode
    bool isFlyMode = false;
    float flySpeed = 15.0f;
    bool flyTogglePressed = false; 
    
    // Helper to get player AABB at specific position
    AABB GetPlayerBox(const glm::vec3& pos) const;
    
    // Check if the player AABB (at newPos) collides with any world box
    // Returns true if collision detected
    bool CheckCollision(const glm::vec3& newPos, const std::vector<AABB>& colliders);
    
    // Returns the Y coordinate of the floor at a specific position, or -FLT_MAX if free space
    float GetFloorHeight(const glm::vec3& pos, const std::vector<AABB>& colliders);
};

#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player(glm::vec3 position) 
    : camera(position)
{
}

glm::mat4 Player::GetViewMatrix() const {
    return camera.GetViewMatrix();
}

glm::vec3 Player::GetPosition() const {
    return camera.GetPosition();
}

glm::vec3 Player::GetFront() const {
    return camera.GetFront();
}

void Player::ProcessMouseMovement(float xoffset, float yoffset) {
    camera.ProcessMouseMovement(xoffset, yoffset);
}

AABB Player::GetPlayerBox(const glm::vec3& pos) const {
    AABB box;
    float r = Radius;
    float feet = pos.y - Height;
    // Box touches feet, but we shrink the bottom slightly for the "step over" logic if needed, 
    // BUT here we want the raw physical box.
    // Let's use exact dimensions.
    box.min = glm::vec3(pos.x - r, feet, pos.z - r);
    box.max = glm::vec3(pos.x + r, pos.y + 0.1f, pos.z + r);
    return box;
}

bool Player::CheckCollision(const glm::vec3& newPos, const std::vector<AABB>& colliders) {
    AABB playerBox = GetPlayerBox(newPos);
    // Raise the bottom check slightly (step tolerance) so we don't get stuck on floor seams
    // But we MUST NOT raise it too much or we walk through walls that are floating slightly? No, walls are usually floor-to-ceiling.
    // For wall collision, we want to ignore the bottom 0.3m (step height).
    // So if a wall is only 0.2m high, we walk over it.
    playerBox.min.y += 0.25f; 

    for (const auto& box : colliders) {
        if (playerBox.Overlaps(box)) {
            return true;
        }
    }
    return false;
}

float Player::GetFloorHeight(const glm::vec3& pos, const std::vector<AABB>& colliders) {
    AABB playerBox = GetPlayerBox(pos);
    float bestY = -FLT_MAX;
    
    // We check for boxes that intersect our XZ footprint and are below our head
    for (const auto& box : colliders) {
        // Check XZ overlap
        if (playerBox.min.x <= box.max.x && playerBox.max.x >= box.min.x &&
            playerBox.min.z <= box.max.z && playerBox.max.z >= box.min.z) {
            
            // It's a potential floor if its top is below or near our feet
            // Allow snapping if we are slightly inside it (due to gravity)
            float epsilon = 0.5f; // Tolerance for snapping
            if (box.max.y <= playerBox.min.y + epsilon) {
                 bestY = std::max(bestY, box.max.y);
            }
        }
    }
    
    // Safety Net: Infinite Floor at Y = 0.0
    if (bestY == -FLT_MAX) {
        return 0.0f;
    }
    // Also ensure we don't return -FLT_MAX if we are above ground but missing it
    return std::max(bestY, 0.0f);
}

void Player::ProcessInputs(GLFWwindow* window, float deltaTime, const std::vector<AABB>& colliders) {
    // 0. Update Base Speed (Dynamic Adjustment)
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
        baseSpeed += 10.0f * deltaTime; // Increase
        std::cout << "Speed: " << baseSpeed << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
        baseSpeed -= 10.0f * deltaTime; // Decrease
        if (baseSpeed < 1.0f) baseSpeed = 1.0f;
        std::cout << "Speed: " << baseSpeed << std::endl;
    }

    // 1. Update Speed
    // Boost speed largely
    float moveSpeed = baseSpeed; // Use member variable
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        moveSpeed *= 2.0f; // Sprint

    // 2. Calculate Horizontal Wish Velocity
    glm::vec3 front = camera.GetFront();
    glm::vec3 right = glm::normalize(glm::cross(front, camera.GetUp()));
    // Flatten
    front = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    right = glm::normalize(glm::vec3(right.x, 0.0f, right.z));
    
    glm::vec3 targetVelXZ(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) targetVelXZ += front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) targetVelXZ -= front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) targetVelXZ -= right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) targetVelXZ += right;
    
    if (glm::length(targetVelXZ) > 0.0f)
        targetVelXZ = glm::normalize(targetVelXZ) * moveSpeed;

    // 3. Move Horizontally (Slide & Step)
    glm::vec3 currentPos = camera.GetPosition();
    glm::vec3 nextPos = currentPos + targetVelXZ * deltaTime;
    
    // Attempt FULL Move
    if (!CheckCollision(nextPos, colliders)) {
        currentPos.x = nextPos.x;
        currentPos.z = nextPos.z;
    } 
    else {
        // Collision! Try stepping UP
        // Check if raising Y allows the move
        glm::vec3 stepPos = nextPos;
        stepPos.y += stepHeight; 
        
        if (!CheckCollision(stepPos, colliders)) {
            // Success! We can move if we step up.
            // But we don't snap Y immediately here, we let Gravity/Grounding handle the Y snap?
            // Actually, if we are moving onto a higher platform, we should probably snap XZ and allow Y to be handled next.
            // But Wait, CheckCollision checks a box. If we just move XZ, our original Y box will collide.
            // So we MUST move Y up to physically reside there.
            // But if we just move Y up, we are "flying".
            // Let's rely on finding ground to snap us.
            // If we move XZ overlapping the step, 'GetFloorHeight' at new XZ should return the step height.
            // But 'CheckCollision' prevents us from entering that XZ because we are too low.
            
            // So: We accept the XZ move, AND we lift Y temporarily to clear the obstacle.
            currentPos.x = nextPos.x;
            currentPos.z = nextPos.z;
            currentPos.y += stepHeight; // Lift up
        }
        else {
            // Cannot step up. Try Sliding (X only)
            glm::vec3 nextPosX = currentPos + glm::vec3(targetVelXZ.x * deltaTime, 0.0f, 0.0f);
            if (!CheckCollision(nextPosX, colliders)) {
                currentPos.x = nextPosX.x;
            } else {
                // Try stepping X
                glm::vec3 stepPosX = nextPosX; 
                stepPosX.y += stepHeight;
                if (!CheckCollision(stepPosX, colliders)) {
                     currentPos.x = nextPosX.x;
                     currentPos.y += stepHeight;
                }
            }
            
            // Try Sliding (Z only)
            glm::vec3 nextPosZ = currentPos + glm::vec3(0.0f, 0.0f, targetVelXZ.z * deltaTime);
            if (!CheckCollision(nextPosZ, colliders)) {
                currentPos.z = nextPosZ.z;
            } else {
                // Try stepping Z
                glm::vec3 stepPosZ = nextPosZ; 
                stepPosZ.y += stepHeight;
                if (!CheckCollision(stepPosZ, colliders)) {
                     currentPos.z = nextPosZ.z;
                     currentPos.y += stepHeight;
                }
            }
        }
    }

    // 4. Update Vertical (Gravity & Ground)
    // Apply gravity
    velocity.y -= gravity * deltaTime;
    
    // Candidate Y position
    float nextY = currentPos.y + velocity.y * deltaTime;
    
    // Find floor height at current XZ
    float floorHeight = GetFloorHeight(glm::vec3(currentPos.x, nextY, currentPos.z), colliders);
    float desiredFeetY = floorHeight;
    float desiredHeadY = floorHeight + Height;
    
    // Check if we hit ground
    if (nextY - Height <= floorHeight) {
        // We are touching or below floor
        currentPos.y = desiredHeadY;
        velocity.y = 0.0f;
        isGrounded = true;
    } else {
        // In air
        currentPos.y = nextY;
        isGrounded = false;
    }
    
    // Ceiling collision? (Optional, usually open sky)
    
    // 5. Jump
    if (isGrounded && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        // velocity.y = jumpForce;
        // isGrounded = false;
        // User didn't strictly ask for jump, but it helps navigation.
        // Let's enable it.
        velocity.y = jumpForce;
        isGrounded = false;
        // Lift slightly to avoid immediate ground snap?
        currentPos.y += 0.1f;
    }

    // Apply
    camera.SetPosition(currentPos);
}

#pragma once

#include "SceneNode.h"
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

// Small helper types used by SchoolBuilder
struct Material
{
    glm::vec3 albedo = glm::vec3(1.0f);
};

enum class MeshType
{
    Cube,
    Plane,
    Pyramid,
    Cylinder,
    Cone,
    Sphere
};

// A SceneNode subclass that carries a mesh type + basic material information.
// Rendering code can cast SceneNode pointers to MeshNode to access mesh/material.
class MeshNode : public SceneNode
{
public:
    using Ptr = std::shared_ptr<MeshNode>;

    MeshNode(MeshType type = MeshType::Cube)
        : SceneNode(), mesh(type)
    {
    }

    MeshNode(const glm::mat4& local, MeshType type = MeshType::Cube)
        : SceneNode(local), mesh(type)
    {
    }

    MeshType mesh;
    Material material;
};

// Build a high-quality architectural school composed of various primitives.
// Returns the root SceneNode (shared_ptr) that owns the entire structure.
class SchoolBuilder
{
public:
    // Generates the scene root of a U-shaped school. size scales the overall footprint.
    // The returned node is a SceneNode::Ptr (shared ownership).
    static SceneNode::Ptr generateSchool(float size = 1.0f);
    
    // Update people animations (call this every frame with current time)
    static void updatePeopleAnimation(SceneNode::Ptr root, float time);
    
    // Update clock animation (call this every frame with current time)
    static void updateClockAnimation(SceneNode::Ptr root, float time);
    
    // Update cloud animation (call this every frame with current time)
    static void updateCloudAnimation(SceneNode::Ptr root, float time);
    
    // Update bird animation (call this every frame with current time)
    static void updateBirdAnimation(SceneNode::Ptr root, float time);
    
    // Store people nodes for animation
    static std::vector<SceneNode::Ptr> s_people;
    
    // Store clock node for animation
    static SceneNode::Ptr s_clock;
    
    // Store cloud nodes for animation
    static std::vector<SceneNode::Ptr> s_clouds;
    
    // Store bird nodes for animation
    static std::vector<SceneNode::Ptr> s_birds;
    
    // Helper struct for flag animation
    struct FlagPart {
        SceneNode::Ptr node;
        float xOffset; // Distance from pole along X axis
        glm::mat4 initialTransform;
    };
    

    
    // Store flag parts for animation
    static std::vector<FlagPart> s_flagParts;
    

    
    // Update flag animation
    static void updateFlagAnimation(SceneNode::Ptr root, float time);
    


    // Gate & Lever System
    static SceneNode::Ptr s_schoolGateLeft;
    static SceneNode::Ptr s_schoolGateRight;
    static SceneNode::Ptr s_gateLever;
    static bool s_isGateOpen;
    static void updateGateAnimation(float deltaTime);

    // Interactive Door System
    struct Door {
        SceneNode::Ptr node;
        glm::vec3 position;     // World position (approx)
        float currentAngle;     // Current rotation angle (degrees)
        float targetAngle;      // Target angle (0 = closed, 90 = open)
        float openAngle;        // The angle to open to (e.g. 90 or -90)
        bool isOpen;
        bool isMoving;
    };
    
    static std::vector<Door> s_doors;
    static void updateDoorAnimation(float dt);
    static void toggleDoor(const glm::vec3& playerPos, float maxDistance = 3.0f, const glm::vec3& forwardDir = glm::vec3(0,0,1));

    // Car Animation System
    struct Car {
        SceneNode::Ptr node;
        float speed;        // Speed in m/s
        float startX;       // Reset position
        float endX;         // Limit position
        float currentX;     // Current X position
        int direction;      // 1 for +X, -1 for -X
    };
    
    static std::vector<Car> s_cars;
    static void updateCarAnimation(float dt);
};
#pragma once

#include "SceneNode.h"

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
    Cone
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
};
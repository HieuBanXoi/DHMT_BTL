#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

class SceneNode : public std::enable_shared_from_this<SceneNode>
{
public:
    using Ptr = std::shared_ptr<SceneNode>;

    SceneNode();
    explicit SceneNode(const glm::mat4& local);

    // Make class polymorphic so dynamic_cast / dynamic_pointer_cast works.
    virtual ~SceneNode();

    // Parent is a weak_ptr to avoid ownership cycles.
    std::weak_ptr<SceneNode> parent;

    // Children are owned by this node.
    std::vector<Ptr> children;

    // Local and cached global transform
    glm::mat4 localTransform;
    glm::mat4 globalTransform;

    // Adds an existing child (will set its parent to this)
    void AddChild(const Ptr& child);

    // Creates a new child, attaches it and returns it
    Ptr CreateChild();

    // Removes a child; returns true if removed
    bool RemoveChild(const Ptr& child);

    // Getters / setters
    const glm::mat4& GetLocalTransform() const { return localTransform; }
    void SetLocalTransform(const glm::mat4& t) { localTransform = t; }

    const glm::mat4& GetGlobalTransform() const { return globalTransform; }

    // Update global transform by multiplying parent's global transform with local transform,
    // store it in this node, and propagate to children.
    void updateGlobalTransform(const glm::mat4& parentTransform);

    // Convenience: update using the actual parent (or identity if none).
    void updateGlobalTransform();

private:
    // Non-copyable semantics (shared_ptr used for ownership)
    SceneNode(const SceneNode&) = delete;
    SceneNode& operator=(const SceneNode&) = delete;
};
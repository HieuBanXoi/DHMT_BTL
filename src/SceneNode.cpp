#include "SceneNode.h"
#include <algorithm>

SceneNode::SceneNode()
    : localTransform(1.0f),
      globalTransform(1.0f)
{
}

SceneNode::SceneNode(const glm::mat4& local)
    : localTransform(local),
      globalTransform(1.0f)
{
}

SceneNode::~SceneNode() = default;

void SceneNode::AddChild(const Ptr& child)
{
    if (!child) return;
    // Avoid adding the same shared_ptr twice.
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) return;

    children.push_back(child);
    child->parent = shared_from_this();
}

SceneNode::Ptr SceneNode::CreateChild()
{
    Ptr child = std::make_shared<SceneNode>();
    children.push_back(child);
    child->parent = shared_from_this();
    return child;
}

bool SceneNode::RemoveChild(const Ptr& child)
{
    if (!child) return false;
    auto it = std::find(children.begin(), children.end(), child);
    if (it == children.end()) return false;

    // clear parent and erase
    (*it)->parent.reset();
    children.erase(it);
    return true;
}

void SceneNode::updateGlobalTransform(const glm::mat4& parentTransform)
{
    // global = parent * local
    globalTransform = parentTransform * localTransform;

    // propagate to children
    for (auto& c : children)
    {
        if (c) c->updateGlobalTransform(globalTransform);
    }
}

void SceneNode::updateGlobalTransform()
{
    if (auto p = parent.lock())
    {
        updateGlobalTransform(p->GetGlobalTransform());
    }
    else
    {
        updateGlobalTransform(glm::mat4(1.0f));
    }
}
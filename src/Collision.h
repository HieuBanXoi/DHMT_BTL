#pragma once

#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <cfloat>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    bool Overlaps(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }
};

// Transform a unit cube [-0.5, 0.5] by a matrix to get a world AABB
inline AABB GetAABBFromTransform(const glm::mat4& transform) {
    // 8 corners of a unit cube centered at origin
    glm::vec3 corners[8] = {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3( 0.5f, -0.5f, -0.5f),
        glm::vec3( 0.5f,  0.5f, -0.5f),
        glm::vec3(-0.5f,  0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f,  0.5f),
        glm::vec3( 0.5f, -0.5f,  0.5f),
        glm::vec3( 0.5f,  0.5f,  0.5f),
        glm::vec3(-0.5f,  0.5f,  0.5f)
    };

    AABB box;
    box.min = glm::vec3(FLT_MAX);
    box.max = glm::vec3(-FLT_MAX);

    for (int i = 0; i < 8; ++i) {
        glm::vec4 worldPos = transform * glm::vec4(corners[i], 1.0f);
        box.min = glm::min(box.min, glm::vec3(worldPos));
        box.max = glm::max(box.max, glm::vec3(worldPos));
    }

    return box;
}

#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"

struct Particle {
    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec4 Color;
    float Life;
};

class ParticleSystem {
public:
    ParticleSystem(unsigned int amount);
    void Update(float dt, unsigned int newParticles, glm::vec3 offset = glm::vec3(0.0f));
    void Draw();
    void Init();

    std::vector<Particle> particles;
    unsigned int amount;
    unsigned int VAO, VBO;
    
    // Config
    glm::vec3 SpawnPosition;
    glm::vec3 Gravity;
};

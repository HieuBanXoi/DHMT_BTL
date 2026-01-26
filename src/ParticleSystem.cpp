#include "ParticleSystem.h"
#include <cstdlib>
#include <ctime>

ParticleSystem::ParticleSystem(unsigned int amount)
    : amount(amount), SpawnPosition(0.0f), Gravity(0.0f, -9.8f, 0.0f)
{
    this->Init();
}

void ParticleSystem::Init()
{
    // Setup VAO/VBO
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    
    // Fill particles with default data
    for (unsigned int i = 0; i < this->amount; ++i)
        this->particles.push_back(Particle());
}

void ParticleSystem::Update(float dt, unsigned int newParticles, glm::vec3 offset)
{
    // Add new particles
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int unusedParticle = -1;
        // Find first unused/dead particle
        for (unsigned int j = 0; j < amount; ++j) {
            if (particles[j].Life <= 0.0f) {
                unusedParticle = j;
                break;
            }
        }
        
        if (unusedParticle >= 0) {
            // Pick a random behavior
            bool isEdgeDrip = (rand() % 100) < 30; // 30% chance for cascading drips

            if (isEdgeDrip) {
                // FALLING FROM EDGE
                // Spawn in a ring around the upper tier (Radius ~ 1.2)
                float angle = (float)(rand() % 360);
                float radius = 1.2f;
                float dX = std::cos(glm::radians(angle)) * radius;
                float dZ = std::sin(glm::radians(angle)) * radius;
                
                // Spawn slightly lower (at the rim level)
                particles[unusedParticle].Position = SpawnPosition + glm::vec3(dX, -0.6f, dZ);
                particles[unusedParticle].Velocity = glm::vec3(dX * 0.2f, -1.0f, dZ * 0.2f); // Fall down and slightly out
                particles[unusedParticle].Color = glm::vec4(0.7f, 0.85f, 1.0f, 0.8f); // Slightly transparent
                particles[unusedParticle].Life = 1.5f;
            } else {
                // CENTRAL JET
                // Reduced height as requested (was 6.0-10.0, now 5.0-7.5)
                float rX = ((rand() % 100) / 100.0f - 0.5f) * 0.6f; 
                float rZ = ((rand() % 100) / 100.0f - 0.5f) * 0.6f; 
                float rY = ((rand() % 100) / 100.0f) * 2.5f + 5.0f; // 5.0 - 7.5
                
                particles[unusedParticle].Position = SpawnPosition + offset;
                particles[unusedParticle].Velocity = glm::vec3(rX, rY, rZ);
                particles[unusedParticle].Color = glm::vec4(0.6f, 0.8f, 1.0f, 1.0f);
                particles[unusedParticle].Life = 2.0f;
            }
        }
    }

    // Update all particles
    for (unsigned int i = 0; i < amount; ++i)
    {
        Particle& p = particles[i];
        p.Life -= dt;
        if (p.Life > 0.0f)
        {
            p.Velocity += Gravity * dt;
            p.Position += p.Velocity * dt;
            p.Color.a -= dt * 0.5f; // Fade out
            
            // Collision Logic
            // Update bound (approx tier 2 level)
            if (p.Position.y < 5.8f) {
                 p.Position.y = 5.8f;
                 p.Velocity.y = -p.Velocity.y * 0.2f; // Small splash
                 p.Velocity.x *= 0.6f;
                 p.Velocity.z *= 0.6f;
            }
        }
    }
}

void ParticleSystem::Draw()
{
    // Collect active particle positions
    std::vector<float> data;
    for (const auto& p : particles) {
        if (p.Life > 0.0f) {
            data.push_back(p.Position.x);
            data.push_back(p.Position.y);
            data.push_back(p.Position.z);
        }
    }
    
    if (data.empty()) return;

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glDrawArrays(GL_POINTS, 0, data.size() / 3);
    glBindVertexArray(0);
}

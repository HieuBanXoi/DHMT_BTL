// ============================================================================
// WaterParticleSystem.h
// Hệ thống hạt nước cho đài phun nước - Header file
// ============================================================================

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "Shader.h"

// Cấu trúc dữ liệu cho một hạt nước
struct WaterParticle {
    glm::vec3 initialPosition;  // Vị trí vòi phun (world space)
    glm::vec3 initialVelocity;  // Vận tốc ban đầu (m/s)
    float startTime;            // Thời điểm bắt đầu (giây)
};

class WaterParticleSystem {
public:
    // Constructor: Tạo hệ thống hạt tại vị trí vòi phun
    WaterParticleSystem(glm::vec3 fountainPosition, int numParticles = 1000);
    
    // Destructor
    ~WaterParticleSystem();
    
    // Render hệ thống hạt
    void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection, float currentTime);
    
    // Cập nhật vị trí vòi phun
    void setFountainPosition(const glm::vec3& position);
    
private:
    // Khởi tạo OpenGL buffers
    void setupBuffers();
    
    // Tạo dữ liệu particles
    void generateParticles();
    
    // Dữ liệu
    std::vector<WaterParticle> m_particles;
    glm::vec3 m_fountainPosition;
    int m_numParticles;
    
    // OpenGL objects
    GLuint m_VAO;
    GLuint m_VBO;
};

// ============================================================================
// WaterParticleSystem.cpp
// Hệ thống hạt nước cho đài phun nước - Implementation
// ============================================================================

#include "WaterParticleSystem.h"
#include <cstdlib>
#include <ctime>

WaterParticleSystem::WaterParticleSystem(glm::vec3 fountainPosition, int numParticles)
    : m_fountainPosition(fountainPosition)
    , m_numParticles(numParticles)
    , m_VAO(0)
    , m_VBO(0)
{
    // Seed random number generator
    srand(static_cast<unsigned>(time(nullptr)));
    
    // Tạo particles
    generateParticles();
    
    // Setup OpenGL buffers
    setupBuffers();
}

WaterParticleSystem::~WaterParticleSystem()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void WaterParticleSystem::generateParticles()
{
    m_particles.clear();
    m_particles.reserve(m_numParticles);
    
    const float PARTICLE_LIFETIME = 2.5f;  // Phải khớp với shader
    
    for (int i = 0; i < m_numParticles; ++i) {
        WaterParticle p;
        
        // Vị trí bắt đầu = vị trí vòi phun
        p.initialPosition = m_fountainPosition;
        
        // Vận tốc ban đầu: phun lên cao + tỏa ra xung quanh
        float angle = (rand() / (float)RAND_MAX) * 2.0f * 3.14159f;
        float horizontalSpeed = (rand() / (float)RAND_MAX) * 2.5f;  // 0-2.5 m/s
        float verticalSpeed = 8.0f + (rand() / (float)RAND_MAX) * 3.0f;  // 8-11 m/s
        
        p.initialVelocity = glm::vec3(
            cos(angle) * horizontalSpeed,  // X: tỏa ngang
            verticalSpeed,                 // Y: phun lên
            sin(angle) * horizontalSpeed   // Z: tỏa ngang
        );
        
        // Thời điểm bắt đầu phân bố đều trong vòng đời
        p.startTime = (i / (float)m_numParticles) * PARTICLE_LIFETIME;
        
        m_particles.push_back(p);
    }
}

void WaterParticleSystem::setupBuffers()
{
    // Tạo VAO và VBO
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    
    // Upload dữ liệu
    glBufferData(GL_ARRAY_BUFFER, 
                 m_particles.size() * sizeof(WaterParticle),
                 m_particles.data(), 
                 GL_STATIC_DRAW);
    
    // Attribute 0: initialPosition (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                          sizeof(WaterParticle), 
                          (void*)offsetof(WaterParticle, initialPosition));
    glEnableVertexAttribArray(0);
    
    // Attribute 1: initialVelocity (vec3)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
                          sizeof(WaterParticle), 
                          (void*)offsetof(WaterParticle, initialVelocity));
    glEnableVertexAttribArray(1);
    
    // Attribute 2: startTime (float)
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 
                          sizeof(WaterParticle), 
                          (void*)offsetof(WaterParticle, startTime));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void WaterParticleSystem::render(Shader& shader, 
                                  const glm::mat4& view, 
                                  const glm::mat4& projection, 
                                  float currentTime)
{
    // Enable blending cho độ trong suốt
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Enable point size từ shader
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    // Disable depth writing (nhưng vẫn depth test)
    // Để particles trong suốt render đúng
    glDepthMask(GL_FALSE);
    
    // Sử dụng shader
    shader.use();
    
    // Set uniforms
    shader.setFloat("u_time", currentTime);
    shader.setMat4("u_model", glm::mat4(1.0f));  // Identity matrix
    shader.setMat4("u_view", view);
    shader.setMat4("u_projection", projection);
    
    // Render particles
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, m_particles.size());
    glBindVertexArray(0);
    
    // Restore state
    glDepthMask(GL_TRUE);
    glDisable(GL_PROGRAM_POINT_SIZE);
}

void WaterParticleSystem::setFountainPosition(const glm::vec3& position)
{
    if (m_fountainPosition != position) {
        m_fountainPosition = position;
        generateParticles();  // Tạo lại particles với vị trí mới
        setupBuffers();       // Upload lại dữ liệu
    }
}

// ============================================================================
// CÁCH SỬ DỤNG TRONG MAIN.CPP
// ============================================================================

/*
// 1. Khai báo (trong class hoặc global)
Shader waterParticleShader;
std::vector<WaterParticleSystem*> fountainParticles;

// 2. Khởi tạo (trong setup)
waterParticleShader = Shader("glsl shaders/water_particle.vs", 
                              "glsl shaders/water_particle.fs");

// Tạo particle system cho mỗi vòi phun
// Ví dụ: 3 vòi phun ở các vị trí khác nhau
fountainParticles.push_back(new WaterParticleSystem(glm::vec3(0.0f, 2.0f, 0.0f), 1000));
fountainParticles.push_back(new WaterParticleSystem(glm::vec3(5.0f, 2.0f, 0.0f), 800));
fountainParticles.push_back(new WaterParticleSystem(glm::vec3(-5.0f, 2.0f, 0.0f), 800));

// 3. Render (trong render loop)
for (auto* particleSystem : fountainParticles) {
    particleSystem->render(waterParticleShader, view, projection, currentTime);
}

// 4. Cleanup (trong destructor hoặc cleanup)
for (auto* particleSystem : fountainParticles) {
    delete particleSystem;
}
fountainParticles.clear();
*/

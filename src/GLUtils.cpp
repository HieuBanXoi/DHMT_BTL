#include "GLUtils.h"

#include <cstddef>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Vertex layout: position (3 floats), normal (3 floats), texcoord (2 floats) -> 8 floats per vertex.

// Cube (36 vertices)
static const float kCubeVertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f,

    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

    // Left face
    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

    // Right face
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,  0.0f, 1.0f,

    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
};

GLuint createCubeVAO()
{
    GLuint VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kCubeVertices), kCubeVertices, GL_STATIC_DRAW);

    constexpr GLsizei stride = static_cast<GLsizei>(8 * sizeof(float));

    // position attribute (location = 0): vec3
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));

    // normal attribute (location = 1): vec3
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));

    // texcoord attribute (location = 2): vec2
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(6 * sizeof(float)));

    // Unbind VAO (VBO stays bound to VAO state)
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return VAO;
}

// Plane (two triangles, 6 vertices): XZ plane centered at origin, normal = +Y
static const float kPlaneVertices[] = {
    // positions           // normals       // texcoords
    -0.5f, 0.0f, -0.5f,    0.0f,1.0f,0.0f,   0.0f, 0.0f,
     0.5f, 0.0f, -0.5f,    0.0f,1.0f,0.0f,   1.0f, 0.0f,
     0.5f, 0.0f,  0.5f,    0.0f,1.0f,0.0f,   1.0f, 1.0f,

     0.5f, 0.0f,  0.5f,    0.0f,1.0f,0.0f,   1.0f, 1.0f,
    -0.5f, 0.0f,  0.5f,    0.0f,1.0f,0.0f,   0.0f, 1.0f,
    -0.5f, 0.0f, -0.5f,    0.0f,1.0f,0.0f,   0.0f, 0.0f
};

GLuint createPlaneVAO()
{
    GLuint VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kPlaneVertices), kPlaneVertices, GL_STATIC_DRAW);

    constexpr GLsizei stride = static_cast<GLsizei>(8 * sizeof(float));

    // position (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));

    // normal (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));

    // texcoord (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(6 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return VAO;
}

// Pyramid: 5 faces (4 triangular sides + 1 square base) = 18 vertices
GLuint createPyramidVAO()
{
    std::vector<float> vertices;
    
    // Base vertices (y = -0.5)
    float base = -0.5f;
    float top = 0.5f;
    float half = 0.5f;
    
    // Apex
    float apexX = 0.0f, apexY = top, apexZ = 0.0f;
    
    // Base corners
    float v0[] = {-half, base, -half};
    float v1[] = { half, base, -half};
    float v2[] = { half, base,  half};
    float v3[] = {-half, base,  half};
    
    auto addVertex = [&](float x, float y, float z, float nx, float ny, float nz) {
        vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
        vertices.push_back(nx); vertices.push_back(ny); vertices.push_back(nz);
        vertices.push_back(0.5f); vertices.push_back(0.5f); // dummy texcoords
    };
    
    // Front face (v0, v1, apex)
    float n0x = 0.0f, n0y = 0.447f, n0z = -0.894f; // approximate normal
    addVertex(v0[0], v0[1], v0[2], n0x, n0y, n0z);
    addVertex(v1[0], v1[1], v1[2], n0x, n0y, n0z);
    addVertex(apexX, apexY, apexZ, n0x, n0y, n0z);
    
    // Right face (v1, v2, apex)
    float n1x = 0.894f, n1y = 0.447f, n1z = 0.0f;
    addVertex(v1[0], v1[1], v1[2], n1x, n1y, n1z);
    addVertex(v2[0], v2[1], v2[2], n1x, n1y, n1z);
    addVertex(apexX, apexY, apexZ, n1x, n1y, n1z);
    
    // Back face (v2, v3, apex)
    float n2x = 0.0f, n2y = 0.447f, n2z = 0.894f;
    addVertex(v2[0], v2[1], v2[2], n2x, n2y, n2z);
    addVertex(v3[0], v3[1], v3[2], n2x, n2y, n2z);
    addVertex(apexX, apexY, apexZ, n2x, n2y, n2z);
    
    // Left face (v3, v0, apex)
    float n3x = -0.894f, n3y = 0.447f, n3z = 0.0f;
    addVertex(v3[0], v3[1], v3[2], n3x, n3y, n3z);
    addVertex(v0[0], v0[1], v0[2], n3x, n3y, n3z);
    addVertex(apexX, apexY, apexZ, n3x, n3y, n3z);
    
    // Base (2 triangles, normal pointing down)
    addVertex(v0[0], v0[1], v0[2], 0.0f, -1.0f, 0.0f);
    addVertex(v2[0], v2[1], v2[2], 0.0f, -1.0f, 0.0f);
    addVertex(v1[0], v1[1], v1[2], 0.0f, -1.0f, 0.0f);
    
    addVertex(v0[0], v0[1], v0[2], 0.0f, -1.0f, 0.0f);
    addVertex(v3[0], v3[1], v3[2], 0.0f, -1.0f, 0.0f);
    addVertex(v2[0], v2[1], v2[2], 0.0f, -1.0f, 0.0f);
    
    GLuint VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    constexpr GLsizei stride = 8 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return VAO;
}

// Cylinder: Y-axis aligned, radius 0.5, height 1
GLuint createCylinderVAO()
{
    const int segments = 16;
    const float radius = 0.5f;
    const float halfHeight = 0.5f;
    
    std::vector<float> vertices;
    
    auto addVertex = [&](float x, float y, float z, float nx, float ny, float nz) {
        vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
        vertices.push_back(nx); vertices.push_back(ny); vertices.push_back(nz);
        vertices.push_back(0.5f); vertices.push_back(0.5f);
    };
    
    // Side faces
    for (int i = 0; i < segments; ++i) {
        float angle1 = (float)i / segments * 2.0f * M_PI;
        float angle2 = (float)(i + 1) / segments * 2.0f * M_PI;
        
        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);
        
        float nx1 = cos(angle1);
        float nz1 = sin(angle1);
        float nx2 = cos(angle2);
        float nz2 = sin(angle2);
        
        // Triangle 1
        addVertex(x1, -halfHeight, z1, nx1, 0.0f, nz1);
        addVertex(x2, -halfHeight, z2, nx2, 0.0f, nz2);
        addVertex(x1, halfHeight, z1, nx1, 0.0f, nz1);
        
        // Triangle 2
        addVertex(x2, -halfHeight, z2, nx2, 0.0f, nz2);
        addVertex(x2, halfHeight, z2, nx2, 0.0f, nz2);
        addVertex(x1, halfHeight, z1, nx1, 0.0f, nz1);
    }
    
    // Top cap
    for (int i = 0; i < segments; ++i) {
        float angle1 = (float)i / segments * 2.0f * M_PI;
        float angle2 = (float)(i + 1) / segments * 2.0f * M_PI;
        
        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);
        
        addVertex(0.0f, halfHeight, 0.0f, 0.0f, 1.0f, 0.0f);
        addVertex(x1, halfHeight, z1, 0.0f, 1.0f, 0.0f);
        addVertex(x2, halfHeight, z2, 0.0f, 1.0f, 0.0f);
    }
    
    // Bottom cap
    for (int i = 0; i < segments; ++i) {
        float angle1 = (float)i / segments * 2.0f * M_PI;
        float angle2 = (float)(i + 1) / segments * 2.0f * M_PI;
        
        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);
        
        addVertex(0.0f, -halfHeight, 0.0f, 0.0f, -1.0f, 0.0f);
        addVertex(x2, -halfHeight, z2, 0.0f, -1.0f, 0.0f);
        addVertex(x1, -halfHeight, z1, 0.0f, -1.0f, 0.0f);
    }
    
    GLuint VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    constexpr GLsizei stride = 8 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return VAO;
}

// Cone: Y-axis aligned, base radius 0.5, height 1
GLuint createConeVAO()
{
    const int segments = 16;
    const float radius = 0.5f;
    const float halfHeight = 0.5f;
    
    std::vector<float> vertices;
    
    auto addVertex = [&](float x, float y, float z, float nx, float ny, float nz) {
        vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
        vertices.push_back(nx); vertices.push_back(ny); vertices.push_back(nz);
        vertices.push_back(0.5f); vertices.push_back(0.5f);
    };
    
    // Side faces
    for (int i = 0; i < segments; ++i) {
        float angle1 = (float)i / segments * 2.0f * M_PI;
        float angle2 = (float)(i + 1) / segments * 2.0f * M_PI;
        
        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);
        
        // Approximate normal for cone side
        float nx1 = cos(angle1);
        float nz1 = sin(angle1);
        float nx2 = cos(angle2);
        float nz2 = sin(angle2);
        float ny = 0.447f; // normalized component
        
        addVertex(x1, -halfHeight, z1, nx1, ny, nz1);
        addVertex(x2, -halfHeight, z2, nx2, ny, nz2);
        addVertex(0.0f, halfHeight, 0.0f, (nx1+nx2)*0.5f, ny, (nz1+nz2)*0.5f);
    }
    
    // Bottom cap
    for (int i = 0; i < segments; ++i) {
        float angle1 = (float)i / segments * 2.0f * M_PI;
        float angle2 = (float)(i + 1) / segments * 2.0f * M_PI;
        
        float x1 = radius * cos(angle1);
        float z1 = radius * sin(angle1);
        float x2 = radius * cos(angle2);
        float z2 = radius * sin(angle2);
        
        addVertex(0.0f, -halfHeight, 0.0f, 0.0f, -1.0f, 0.0f);
        addVertex(x2, -halfHeight, z2, 0.0f, -1.0f, 0.0f);
        addVertex(x1, -halfHeight, z1, 0.0f, -1.0f, 0.0f);
    }
    
    GLuint VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    constexpr GLsizei stride = 8 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return VAO;
}

// Sphere: UV sphere with latitude/longitude segments
GLuint createSphereVAO()
{
    const int latSegments = 16;
    const int lonSegments = 32;
    const float radius = 0.5f;
    
    std::vector<float> vertices;
    
    auto addVertex = [&](float x, float y, float z, float nx, float ny, float nz) {
        vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
        vertices.push_back(nx); vertices.push_back(ny); vertices.push_back(nz);
        vertices.push_back(0.5f); vertices.push_back(0.5f);
    };
    
    for (int lat = 0; lat < latSegments; ++lat) {
        float theta1 = (float)lat / latSegments * M_PI;
        float theta2 = (float)(lat + 1) / latSegments * M_PI;
        
        for (int lon = 0; lon < lonSegments; ++lon) {
            float phi1 = (float)lon / lonSegments * 2.0f * M_PI;
            float phi2 = (float)(lon + 1) / lonSegments * 2.0f * M_PI;
            
            float x1 = radius * sin(theta1) * cos(phi1);
            float y1 = radius * cos(theta1);
            float z1 = radius * sin(theta1) * sin(phi1);
            
            float x2 = radius * sin(theta1) * cos(phi2);
            float y2 = radius * cos(theta1);
            float z2 = radius * sin(theta1) * sin(phi2);
            
            float x3 = radius * sin(theta2) * cos(phi2);
            float y3 = radius * cos(theta2);
            float z3 = radius * sin(theta2) * sin(phi2);
            
            float x4 = radius * sin(theta2) * cos(phi1);
            float y4 = radius * cos(theta2);
            float z4 = radius * sin(theta2) * sin(phi1);
            
            float nx1 = x1 / radius, ny1 = y1 / radius, nz1 = z1 / radius;
            float nx2 = x2 / radius, ny2 = y2 / radius, nz2 = z2 / radius;
            float nx3 = x3 / radius, ny3 = y3 / radius, nz3 = z3 / radius;
            float nx4 = x4 / radius, ny4 = y4 / radius, nz4 = z4 / radius;
            
            addVertex(x1, y1, z1, nx1, ny1, nz1);
            addVertex(x2, y2, z2, nx2, ny2, nz2);
            addVertex(x3, y3, z3, nx3, ny3, nz3);
            
            addVertex(x1, y1, z1, nx1, ny1, nz1);
            addVertex(x3, y3, z3, nx3, ny3, nz3);
            addVertex(x4, y4, z4, nx4, ny4, nz4);
        }
    }
    
    GLuint VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    constexpr GLsizei stride = 8 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return VAO;
}
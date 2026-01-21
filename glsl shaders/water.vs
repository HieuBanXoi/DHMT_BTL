#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec2 WaveCoord;
out float WaveHeight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main()
{
    // Calculate world position
    vec4 worldPos = model * vec4(aPos, 1.0);
    
    // Create animated waves using multiple sine waves
    float wave1 = sin(worldPos.x * 2.0 + time * 1.5) * 0.02;
    float wave2 = sin(worldPos.z * 3.0 - time * 2.0) * 0.015;
    float wave3 = sin((worldPos.x + worldPos.z) * 1.5 + time * 1.0) * 0.01;
    
    WaveHeight = wave1 + wave2 + wave3;
    
    // Apply wave displacement to Y coordinate
    worldPos.y += WaveHeight;
    
    FragPos = worldPos.xyz;
    
    // Calculate perturbed normal for wave lighting
    vec3 tangent = vec3(1.0, cos(worldPos.x * 2.0 + time * 1.5) * 0.04, 0.0);
    vec3 bitangent = vec3(0.0, cos(worldPos.z * 3.0 - time * 2.0) * 0.045, 1.0);
    Normal = normalize(cross(tangent, bitangent));
    
    // Pass wave coordinates for texture-like effects
    WaveCoord = vec2(worldPos.x * 0.1, worldPos.z * 0.1);
    
    gl_Position = projection * view * worldPos;
}

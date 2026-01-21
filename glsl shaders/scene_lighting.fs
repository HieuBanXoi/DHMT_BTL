#version 330 core

// Inputs (from vertex shader) - world-space
in vec3 FragPos;
in vec3 Normal;

// Output
out vec4 FragColor;

// Directional light description
struct DirectionalLight
{
    // Direction that light rays travel (normalized, points FROM light TO scene).
    // Use in lighting: diffuse = max(dot(N, direction), 0.0)
    vec3 direction;
    vec3 color;
    float intensity;
};

// Point light structure
struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
};

// Sun object model matrix (world transform). The sun is expected to orbit the scene center:
// - Its model matrix encodes rotation + translation.
// - We sample the sun's world-space origin to compute its world position.
uniform mat4 sunModel;

// Sun appearance parameters (used to populate the DirectionalLight)
uniform vec3 sunColor;
uniform float sunIntensity;

// Scene center (world-space). If your scene center is at origin, set to vec3(0.0).
uniform vec3 sceneCenter;

// === NEW: Sun and Moon Directional Lights ===
// These lights move with the sun/moon to simulate emission
uniform vec3 sunLightDirection;
uniform vec3 sunLightColor;
uniform float sunLightIntensity;

uniform vec3 moonLightDirection;
uniform vec3 moonLightColor;
uniform float moonLightIntensity;

// Simple material / ambient parameters for demonstration
uniform vec3 ambientColor;
uniform vec3 albedo;

// Point lights (streetlights)
#define MAX_POINT_LIGHTS 20
uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

// Build a DirectionalLight from the sun's model matrix by computing the sun world position
// and deriving the light direction from sun -> sceneCenter.
DirectionalLight CreateDirectionalLightFromSun(mat4 sunModelMatrix, vec3 center)
{
    DirectionalLight light;

    // Sun world position is the transformed origin of the sun object
    vec3 sunWorldPos = (sunModelMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;

    // Direction of incoming light: from sun toward the scene center
    light.direction = normalize(center - sunWorldPos);

    light.color = sunColor;
    light.intensity = sunIntensity;
    return light;
}

// Calculate directional light contribution
vec3 CalculateDirectionalLight(vec3 direction, vec3 color, float intensity, vec3 normal, vec3 albedoColor)
{
    // Diffuse term (Lambert)
    float NdotL = max(dot(normal, direction), 0.0);
    
    vec3 diffuse = color * intensity * NdotL * albedoColor;
    
    return diffuse;
}

// Calculate point light contribution with stronger attenuation for localized effect
vec3 CalculatePointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 albedoColor)
{
    vec3 lightDir = light.position - fragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);
    
    // Stronger attenuation for more localized lighting (increased quadratic term)
    float attenuation = light.intensity / (1.0 + 0.2 * distance + 0.15 * distance * distance);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 diffuse = light.color * diff * albedoColor * attenuation;
    
    return diffuse;
}

void main()
{
    // Normalize interpolated normal
    vec3 N = normalize(Normal);

    // Create the directional light based on the sun transform
    DirectionalLight sun = CreateDirectionalLightFromSun(sunModel, sceneCenter);

    // Diffuse term (Lambert) from sun
    float NdotL = max(dot(N, sun.direction), 0.0);

    vec3 diffuse = sun.color * sun.intensity * NdotL * albedo;

    // Simple ambient
    vec3 ambient = ambientColor * albedo;

    vec3 color = ambient + diffuse;
    
    // === ADD SUN DIRECTIONAL LIGHT ===
    color += CalculateDirectionalLight(sunLightDirection, sunLightColor, sunLightIntensity, N, albedo);
    
    // === ADD MOON DIRECTIONAL LIGHT ===
    color += CalculateDirectionalLight(moonLightDirection, moonLightColor, moonLightIntensity, N, albedo);
    
    // Add point light contributions (streetlights)
    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; ++i)
    {
        color += CalculatePointLight(pointLights[i], FragPos, N, albedo);
    }

    FragColor = vec4(color, 1.0);
}

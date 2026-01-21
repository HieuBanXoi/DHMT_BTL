#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 WaveCoord;
in float WaveHeight;

out vec4 FragColor;

uniform vec3 viewPos;
uniform float time;
uniform vec3 sunLightDirection;
uniform vec3 sunLightColor;
uniform float sunLightIntensity;

// Noise function for procedural texture
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Fractal Brownian Motion for detailed texture
float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for(int i = 0; i < 4; i++) {
        value += amplitude * noise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main()
{
    // === PROCEDURAL WATER TEXTURE ===
    vec2 uv = WaveCoord + time * 0.02; // Slow drift
    
    // Multi-layer noise for water texture
    float waterNoise1 = fbm(uv * 3.0 + time * 0.1);
    float waterNoise2 = fbm(uv * 5.0 - time * 0.15);
    float waterNoise3 = fbm(uv * 8.0 + time * 0.08);
    
    // Combine noises for complex water pattern
    float waterPattern = waterNoise1 * 0.5 + waterNoise2 * 0.3 + waterNoise3 * 0.2;
    
    // === WATER COLOR WITH TEXTURE ===
    vec3 deepWaterColor = vec3(0.05, 0.35, 0.55);    // Deep blue-green
    vec3 shallowWaterColor = vec3(0.25, 0.65, 0.85); // Light cyan
    vec3 foamColor = vec3(0.8, 0.9, 1.0);            // White foam
    
    // Mix colors based on noise pattern and wave height
    float depthFactor = clamp(WaveHeight * 10.0 + waterPattern, 0.0, 1.0);
    vec3 waterColor = mix(deepWaterColor, shallowWaterColor, depthFactor);
    
    // Add foam at wave peaks
    float foamAmount = smoothstep(0.6, 0.8, waterPattern) * 0.3;
    waterColor = mix(waterColor, foamColor, foamAmount);
    
    // === ANIMATED CAUSTICS (light patterns on water) ===
    vec2 causticsUV = WaveCoord * 2.0;
    float caustic1 = sin(causticsUV.x * 10.0 + time * 2.0) * 0.5 + 0.5;
    float caustic2 = sin(causticsUV.y * 12.0 - time * 1.5) * 0.5 + 0.5;
    float caustic3 = sin((causticsUV.x + causticsUV.y) * 8.0 + time * 1.8) * 0.5 + 0.5;
    
    float caustics = (caustic1 * caustic2 + caustic3) * 0.25;
    waterColor += vec3(caustics * 0.4);
    
    // === NORMAL PERTURBATION FOR LIGHTING ===
    // Use noise to perturb normal for more realistic lighting
    vec3 norm = normalize(Normal);
    float normalNoise = fbm(uv * 10.0 + time * 0.2) * 0.1;
    norm.x += normalNoise;
    norm.z += normalNoise * 0.8;
    norm = normalize(norm);
    
    // === FRESNEL EFFECT (edges more reflective) ===
    vec3 viewDir = normalize(viewPos - FragPos);
    float fresnel = pow(1.0 - max(dot(viewDir, norm), 0.0), 3.0);
    
    // === LIGHTING ===
    // Sun light (directional)
    float diff = max(dot(norm, sunLightDirection), 0.0);
    vec3 diffuse = sunLightColor * diff * sunLightIntensity;
    
    // Specular highlights (shiny water surface)
    vec3 reflectDir = reflect(-sunLightDirection, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256.0); // Very shiny
    vec3 specular = sunLightColor * spec * 1.2;
    
    // === SUBSURFACE SCATTERING (light through water) ===
    float subsurface = pow(max(dot(viewDir, -sunLightDirection), 0.0), 4.0) * 0.3;
    vec3 subsurfaceColor = vec3(0.3, 0.7, 0.9) * subsurface;
    
    // === COMBINE ALL EFFECTS ===
    vec3 ambient = vec3(0.3, 0.45, 0.6) * 0.5; // Ambient water color
    vec3 finalColor = waterColor * (ambient + diffuse) + specular + subsurfaceColor;
    
    // Add Fresnel glow at edges (sky reflection)
    vec3 skyColor = vec3(0.5, 0.7, 1.0);
    finalColor += skyColor * fresnel * 0.5;
    
    // Add sparkles on water surface
    float sparkle = pow(noise(uv * 50.0 + time), 10.0) * 0.8;
    finalColor += vec3(sparkle);
    
    // === TRANSPARENCY ===
    // Water is semi-transparent, more opaque at steep viewing angles
    float alpha = 0.75 + fresnel * 0.2;
    
    // Slightly more opaque where there's foam
    alpha += foamAmount * 0.15;
    
    FragColor = vec4(finalColor, alpha);
}

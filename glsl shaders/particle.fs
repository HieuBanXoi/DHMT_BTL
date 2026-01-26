#version 330 core
out vec4 FragColor;

uniform vec4 particleColor;

void main()
{
    // Circular particle
    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    if (dot(circCoord, circCoord) > 1.0) {
        discard;
    }
    FragColor = particleColor;
}

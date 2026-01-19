#pragma once

#include <glad/glad.h>

// Creates and returns a VAO for a unit cube centered at origin.
// The VBO is created and remains bound to the VAO (caller may delete via glDeleteBuffers later if desired).
GLuint createCubeVAO();

// Creates and returns a VAO for a unit plane (XZ) centered at origin, y = 0.
// Layout matches the cube (position, normal, texcoord).
GLuint createPlaneVAO();

// Creates and returns a VAO for a pyramid (square base, apex at top).
// Base centered at origin, height = 1.
GLuint createPyramidVAO();

// Creates and returns a VAO for a cylinder (Y-axis aligned).
// Radius = 0.5, Height = 1, centered at origin.
GLuint createCylinderVAO();

// Creates and returns a VAO for a cone (Y-axis aligned).
// Base radius = 0.5, Height = 1, apex at top.
GLuint createConeVAO();

// Creates and returns a VAO for a UV sphere.
// Radius = 0.5, centered at origin.
GLuint createSphereVAO();
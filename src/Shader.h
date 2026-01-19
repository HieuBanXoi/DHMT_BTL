
#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

// Simple OpenGL shader helper:
// - Loads vertex/fragment GLSL from files
// - Compiles, links and exposes a program ID
// - Utility setters for common uniform types (bool, int, float, vec3, mat4)
class Shader
{
public:
    // Program ID
    unsigned int ID = 0;

    // Construct from file paths (vertex + fragment). Throws std::runtime_error on file IO errors.
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    // Non-copyable (shader programs should be unique). Movable for convenience.
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    ~Shader();

    // Activate the shader
    void Use() const;

    // Uniform helpers
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;
    void SetMat4(const std::string& name, const glm::mat4& mat) const;

private:
    static std::string ReadFile(const std::string& path);
    static void CheckCompileErrors(GLuint object, const std::string& type);
};
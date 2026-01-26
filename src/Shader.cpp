
#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    // 1. Retrieve shader source code from files
    const std::string vertexCode = ReadFile(vertexPath);
    const std::string fragmentCode = ReadFile(fragmentPath);
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Compile shaders
    GLuint vertex, fragment;
    int success = 0;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");

    // 3. Link program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    // 4. Cleanup shader objects (no longer needed after linking)
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(Shader&& other) noexcept
    : ID(other.ID)
{
    other.ID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        if (ID != 0) glDeleteProgram(ID);
        ID = other.ID;
        other.ID = 0;
    }
    return *this;
}

Shader::~Shader()
{
    if (ID != 0)
        glDeleteProgram(ID);
}

void Shader::Use() const
{
    glUseProgram(ID);
}

void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

std::string Shader::ReadFile(const std::string& path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to open shader file: " + path);
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

void Shader::CheckCompileErrors(GLuint object, const std::string& type)
{
    GLint success = 0;
    if (type == "VERTEX" || type == "FRAGMENT")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLint logLen = 0;
            glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLen);
            std::string infoLog(static_cast<size_t>(logLen), '\0');
            glGetShaderInfoLog(object, logLen, nullptr, &infoLog[0]);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
    else if (type == "PROGRAM")
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLint logLen = 0;
            glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logLen);
            std::string infoLog(static_cast<size_t>(logLen), '\0');
            glGetProgramInfoLog(object, logLen, nullptr, &infoLog[0]);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                      << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
    else
    {
        // Unknown type, just query program link status as a fallback
        glGetProgramiv(object, GL_LINK_STATUS, &success);
    }
}
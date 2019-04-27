#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ou {

static void show_source(const char* source)
{
    std::istringstream src(source);
    std::string line;
    int lineno = 0;
    std::cerr << "Source:\n";
    while (std::getline(src, line)) {
        std::cerr << ++lineno << ": " << line << "\n";
    }
}

static GLuint load_shaders(const char* vertex_shader, const char* fragment_shader)
{
    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;
    int infoLogLength;

    // Compile Vertex Shader
    std::clog << "Compiling vertex shader\n";
    glShaderSource(vertexShaderID, 1, &vertex_shader, nullptr);
    glCompileShader(vertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShaderID, infoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        show_source(vertex_shader);
        std::cerr << &VertexShaderErrorMessage[0] << "\n";
        throw std::runtime_error("Error compiling vertex shader");
    }

    // Compile Fragment Shader
    std::clog << "Compiling fragment shader\n";
    glShaderSource(fragmentShaderID, 1, &fragment_shader, nullptr);
    glCompileShader(fragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShaderID, infoLogLength, nullptr, &fragmentShaderErrorMessage[0]);
        show_source(fragment_shader);
        std::cerr << &fragmentShaderErrorMessage[0] << "\n";
        throw std::runtime_error("Error compiling fragment shader");
    }

    // Link the program
    std::clog << "Linking program\n";
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    // Check the program
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programID, infoLogLength, nullptr, &ProgramErrorMessage[0]);
        std::cerr << &ProgramErrorMessage[0] << "\n";
        throw std::runtime_error("Error linking shaders");
    }

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

static GLuint load_comp_shaders(const char* comp_shader)
{
    GLuint compShaderID = glCreateShader(GL_COMPUTE_SHADER);

    // Compile compute shader
    std::clog << "Compiling compute shader\n";
    glShaderSource(compShaderID, 1, &comp_shader, nullptr);
    glCompileShader(compShaderID);

    GLint result = GL_FALSE;
    int infoLogLength;

    // Check Shader
    glGetShaderiv(compShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(compShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> computeShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(compShaderID, infoLogLength, nullptr, &computeShaderErrorMessage[0]);
        show_source(comp_shader);
        std::cerr << &computeShaderErrorMessage[0] << "\n";
        throw std::runtime_error("Error compiling fragment shader");
    }

    // Link the program
    std::clog << "Linking program\n";
    GLuint programID = glCreateProgram();
    glAttachShader(programID, compShaderID);
    glLinkProgram(programID);

    // Check the program
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programID, infoLogLength, nullptr, &ProgramErrorMessage[0]);
        std::cerr << &ProgramErrorMessage[0] << "\n";
        throw std::runtime_error("Error linking shader");
    }

    glDetachShader(programID, compShaderID);

    glDeleteShader(compShaderID);

    return programID;
}

Shader::Shader(const char* vertex_shader, const char* fragment_shader)
    : m_id(load_shaders(vertex_shader, fragment_shader))
{
}

Shader::Shader(const char* comp_shader)
    : m_id(load_comp_shaders(comp_shader))
{
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

Shader::Shader(Shader&& other) noexcept
    : m_id(std::exchange(other.m_id, 0))
{
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    glDeleteProgram(m_id);
    m_id = std::exchange(other.m_id, 0);
    return *this;
}

void Shader::setUniform(GLint location, int value)
{
    glProgramUniform1i(m_id, location, value);
}

void Shader::setUniform(GLint location, float value)
{
    glProgramUniform1f(m_id, location, value);
}

void Shader::setUniform(GLint location, const glm::vec2& vec)
{
    glProgramUniform2f(m_id, location, vec.x, vec.y);
}

void Shader::setUniform(GLint location, const glm::vec3& vec)
{
    glProgramUniform3f(m_id, location, vec.x, vec.y, vec.z);
}

void Shader::setUniform(GLint location, const glm::vec4& vec)
{
    glProgramUniform4f(m_id, location, vec.x, vec.y, vec.z, vec.w);
}

void Shader::setUniform(GLint location, const glm::mat4& mat)
{
    glProgramUniformMatrix4fv(m_id, location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::use() const
{
    glUseProgram(m_id);
}

GLuint Shader::id() const
{
    return m_id;
}
}

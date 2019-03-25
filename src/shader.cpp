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

static GLuint load_shaders(const char* vertex_file_path, const char* fragment_file_path)
{
    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string vertexShaderCode;
    std::ifstream vertexShaderStream(vertex_file_path, std::ios::in);
    if (vertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << vertexShaderStream.rdbuf();
        vertexShaderCode = sstr.str();
        vertexShaderStream.close();
    } else {
        std::cerr << "Impossible to open " << vertex_file_path << "\n";
        throw std::runtime_error("Error opening shader file");
    }

    // Read the Fragment Shader code from the file
    std::string fragmentShaderCode;
    std::ifstream fragmentShaderStream(fragment_file_path, std::ios::in);
    if (fragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << fragmentShaderStream.rdbuf();
        fragmentShaderCode = sstr.str();
        fragmentShaderStream.close();
    } else {
        std::cerr << "Impossible to open " << fragment_file_path << "\n";
        throw std::runtime_error("Error opneing shader file");
    }

    GLint result = GL_FALSE;
    int infoLogLength;

    // Compile Vertex Shader
    std::clog << "Compiling shader : " << vertex_file_path << "\n";
    char const* vertexSourcePointer = vertexShaderCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
    glCompileShader(vertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &VertexShaderErrorMessage[0]);
        std::cerr << &VertexShaderErrorMessage[0] << "\n";
        throw std::runtime_error("Error compiling vertex shader");
    }

    // Compile Fragment Shader
    std::clog << "Compiling shader : " << fragment_file_path << "\n";
    char const* fragmentSourcePointer = fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, NULL);
    glCompileShader(fragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
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
        glGetProgramInfoLog(programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
        std::cerr << &ProgramErrorMessage[0] << "\n";
        throw std::runtime_error("Error linking shaders");
    }

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

static GLuint load_comp_shaders(const char* comp_file_path)
{
    GLuint compShaderID = glCreateShader(GL_COMPUTE_SHADER);

    // Read the Shader code from the file
    std::string compShaderCode;
    std::ifstream compShaderStream(comp_file_path, std::ios::in);
    if (compShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << compShaderStream.rdbuf();
        compShaderCode = sstr.str();
        compShaderStream.close();
    } else {
        std::cerr << "Impossible to open " << comp_file_path << "\n";
        throw std::runtime_error("Error opening shader file");
    }

    // Compile compute shader
    std::clog << "Compiling shader : " << comp_file_path << "\n";
    char const* vertexSourcePointer = compShaderCode.c_str();
    glShaderSource(compShaderID, 1, &vertexSourcePointer, NULL);
    glCompileShader(compShaderID);

    GLint result = GL_FALSE;
    int infoLogLength;

    // Check Shader
    glGetShaderiv(compShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(compShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> computeShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(compShaderID, infoLogLength, NULL, &computeShaderErrorMessage[0]);
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
        glGetProgramInfoLog(programID, infoLogLength, NULL, &ProgramErrorMessage[0]);
        std::cerr << &ProgramErrorMessage[0] << "\n";
        throw std::runtime_error("Error linking shader");
    }

    glDetachShader(programID, compShaderID);

    glDeleteShader(compShaderID);

    return programID;
}

Shader::Shader(const char* vertex_file_path, const char* fragment_file_path)
    : m_id(load_shaders(vertex_file_path, fragment_file_path))
{
}

Shader::Shader(const char* comp_file_path)
    : m_id(load_comp_shaders(comp_file_path))
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

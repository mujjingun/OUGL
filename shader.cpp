#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

namespace ou {

static GLuint load_shaders(const char* vertex_file_path, const char* fragment_file_path)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    } else {
        std::cerr << "Impossible to open " << vertex_file_path << "\n";
        throw std::runtime_error("Error opening shader file");
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    std::clog << "Compiling shader : " << vertex_file_path << "\n";
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        std::cerr << &VertexShaderErrorMessage[0] << "\n";
        throw std::runtime_error("Error compiling vertex shader");
    }

    // Compile Fragment Shader
    std::clog << "Compiling shader : " << fragment_file_path << "\n";
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        std::cerr << &FragmentShaderErrorMessage[0] << "\n";
        throw std::runtime_error("Error compiling fragment shader");
    }

    // Link the program
    std::clog << "Linking program\n";
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        std::cerr << &ProgramErrorMessage[0] << "\n";
        throw std::runtime_error("Error linking shaders");
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

Shader::Shader(const char* vertex_file_path, const char* fragment_file_path)
    : m_id(load_shaders(vertex_file_path, fragment_file_path))
{
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

Shader::Shader(Shader&& other)
    : m_id(std::exchange(other.m_id, 0))
{
}

Shader& Shader::operator=(Shader&& other)
{
    glDeleteProgram(m_id);
    m_id = std::exchange(other.m_id, 0);
}

void Shader::setUniform(GLint location, int value)
{
    glUniform1i(location, value);
}

void Shader::setUniform(GLint location, float value)
{
    glUniform1f(location, value);
}

void Shader::setUniform(GLint location, const glm::vec2& vec)
{
    glUniform2f(location, vec.x, vec.y);
}

void Shader::setUniform(GLint location, const glm::vec3& vec)
{
    glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::setUniform(GLint location, const glm::vec4& vec)
{
    glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void Shader::setUniform(GLint location, const glm::mat4& mat)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
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

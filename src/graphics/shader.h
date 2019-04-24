#ifndef UTILS_H
#define UTILS_H

#include <GL/glew.h>

#include <glm/glm.hpp>

namespace ou {

class Shader {
    GLuint m_id;

public:
    Shader();
    Shader(const char* vertex_shader, const char* fragment_shader);
    Shader(const char* comp_shader);
    ~Shader();

    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void setUniform(GLint location, int value);
    void setUniform(GLint location, float value);
    void setUniform(GLint location, glm::vec2 const& vec);
    void setUniform(GLint location, glm::vec3 const& vec);
    void setUniform(GLint location, glm::vec4 const& vec);
    void setUniform(GLint location, glm::mat4 const& mat);

    void use() const;

    GLuint id() const;
};

}

#endif // UTILS_H

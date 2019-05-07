#ifndef GLQUERY_H
#define GLQUERY_H

#include <GL/glew.h>

namespace ou {

class GLQuery {
    GLuint m_id;

public:
    GLQuery();
    GLQuery(GLenum target);
    ~GLQuery();

    GLQuery(GLQuery const&) = delete;
    GLQuery& operator=(GLQuery const&) = delete;

    GLQuery(GLQuery&& other) noexcept;
    GLQuery& operator=(GLQuery&& other) noexcept;

    GLuint id() const;
};
}

#endif // GLQUERY_H

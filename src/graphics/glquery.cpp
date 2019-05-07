#include "glquery.h"

#include <algorithm>

namespace ou {

GLQuery::GLQuery()
    : m_id(0)
{
}

GLQuery::GLQuery(GLenum target)
{
    glCreateQueries(target, 1, &m_id);
}

GLQuery::~GLQuery()
{
    glDeleteQueries(1, &m_id);
}

GLQuery::GLQuery(GLQuery&& other) noexcept
    : m_id(std::exchange(other.m_id, 0))
{
}

GLQuery& GLQuery::operator=(GLQuery&& other) noexcept
{
    glDeleteQueries(1, &m_id);
    m_id = std::exchange(other.m_id, 0);
    return *this;
}

GLuint GLQuery::id() const
{
    return m_id;
}
}

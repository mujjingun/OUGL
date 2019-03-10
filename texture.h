#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

namespace ou {

class Texture {
    GLuint m_id;

public:
    Texture();
    ~Texture();

    Texture(Texture const&) = delete;
    Texture& operator=(Texture const&) = delete;

    Texture(Texture&& other);
    Texture& operator=(Texture&& other);
};
}

#endif // TEXTURE_H

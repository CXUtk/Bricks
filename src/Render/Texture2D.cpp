#include "Texture2D.h"

Texture2D::Texture2D(GLuint id, glm::ivec2 size) :_id(id), _size(size) {
}

Texture2D::~Texture2D() {
    glDeleteTextures(1, &_id);
}

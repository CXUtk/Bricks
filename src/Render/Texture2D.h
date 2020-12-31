#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class Texture2D {
public:
    Texture2D(GLuint id, glm::ivec2 size);
    ~Texture2D();

    GLuint getId() const { return _id; }
    glm::ivec2 getSize() const { return _size; }

private:
    GLuint _id;
    glm::ivec2 _size;
    //GLuint _internalFormat;
    //GLuint _imageFormat;
    //GLuint _warpS, _warpT;
    //GLuint _filterMin, filterMax;
};

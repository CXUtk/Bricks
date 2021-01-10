#pragma once
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Segment {
    glm::vec2 start, end;
    Segment() {}
    Segment(glm::vec2 start, glm::vec2 end) :start(start), end(end) {}
};

class GLUtils {
public:
    static void genQuad(GLuint& vao, GLuint& vbo, GLuint& ebo);
};

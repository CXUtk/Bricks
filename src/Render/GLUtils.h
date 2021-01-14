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

struct Triangle {
    glm::vec4 v[3];
    Triangle() {}
    Triangle(const glm::vec4& A, const glm::vec4& B, const glm::vec4& C) {
        v[0] = A, v[1] = B, v[2] = C;
    }
};

class GLUtils {
public:
    static void genQuad(GLuint& vao, GLuint& vbo, GLuint& ebo);
    static float getRadians(glm::vec2 dir);
};

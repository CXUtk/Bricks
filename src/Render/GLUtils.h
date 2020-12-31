#pragma once
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GLUtils {
public:
    static void genQuad(GLuint& vao, GLuint& vbo, GLuint& ebo);
};

#include "GLUtils.h"

// 不会变的顶点列表
static unsigned int indices[] = {
    0, 1, 3,
    0, 2, 1,
};

void GLUtils::genQuad(GLuint& vao, GLuint& vbo, GLuint& ebo) {
    // 准备绘制一个Quad需要的顶点缓存序列
    // 一个Quad由4个顶点和两个三角形组成
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);


    glBindVertexArray(vao);

    // 绑定VAO到四个顶点
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 4, nullptr, GL_DYNAMIC_DRAW);

    // 绑定顶点序列到indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(0);

    // 解除当前绑定
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

float GLUtils::getRadians(glm::vec2 dir) {
    return std::atan2(dir.y, dir.x);
}

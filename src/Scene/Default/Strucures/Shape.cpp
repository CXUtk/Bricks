#include "Shape.h"
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
#include "Game.h"

Shape::Shape(int rows, int cols, const char* shape) :rows(rows), cols(cols) {
    bits = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int id = i * cols + j;
            if (shape[id] == 'O') {
                bits.set(id);
            }
        }
    }
}

Shape Shape::rotateClockwise() const {
    Shape out(cols, rows);
    out.bits = 0;
    for (int i = rows - 1; i >= 0; i--) {
        for (int j = 0; j < cols; j++) {
            int id = i * cols + j;
            if (bits.test(id)) {
                int id2 = j * rows + (rows - i - 1);
                out.bits.set(id2);
            }
        }
    }
    return out;
}

Shape Shape::flip() const {
    Shape out(rows, cols);
    out.bits = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int id = i * cols + j;
            if (bits.test(id)) {
                int id2 = i * cols + (cols - j - 1);
                out.bits.set(id2);
            }
        }
    }
    return out;
}

int Shape::count() const {
    int cnt = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int id = i * cols + j;
            if (bits.test(id)) {
                cnt++;
            }
        }
    }
    return cnt;
}

std::shared_ptr<Texture2D> Shape::generateTexture(glm::vec3 color) const {
    constexpr int BLOCK_SIZE_DRAW = 32;
    auto& game = Game::GetInstance();

    // 生成一个临时framebuffer，然后绘制物块，最后把framebuffer保存为纹理
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // 绑定纹理
    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    int width = cols * BLOCK_SIZE_DRAW;
    int height = rows * BLOCK_SIZE_DRAW;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cols * BLOCK_SIZE_DRAW, rows * BLOCK_SIZE_DRAW, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    auto savedProj = game.getGraphics()->getProjectionMatrix();


    auto proj = glm::scale(glm::vec3(1, -1, 1)) * glm::ortho(0.f, (float)cols * BLOCK_SIZE_DRAW, 0.f, (float)rows * BLOCK_SIZE_DRAW, -1.0f, 1.0f);
    game.getGraphics()->setProjectionMatrix(proj);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int id = i * cols + j;
            if (bits.test(id)) {
                auto pos = glm::vec2(j * BLOCK_SIZE_DRAW, i * BLOCK_SIZE_DRAW);
                game.getGraphics()->drawQuad(pos, glm::vec2(BLOCK_SIZE_DRAW), color);
            }
        }
    }

    // 解除绑定
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glViewport(0, 0, game.getWidth(), game.getHeight());
    game.getGraphics()->setProjectionMatrix(savedProj);

    return std::shared_ptr<Texture2D>(new Texture2D(texID, glm::ivec2(cols * BLOCK_SIZE_DRAW, rows * BLOCK_SIZE_DRAW)));
}

#include "../include/JSpriteRenderer.h"

JSpriteRenderer::JSpriteRenderer(JShader &shader) {
    this->shader = shader;
    initRenderData();

    shader.Use();
    modelLocation = glGetUniformLocation(shader.Program, "model");
    spriteRectLocation = glGetUniformLocation(shader.Program, "spriteRect");
    textureSizeLocation = glGetUniformLocation(shader.Program, "textureSize");
    colorLocation = glGetUniformLocation(shader.Program, "color");
    flippedLocation = glGetUniformLocation(shader.Program, "flipped");
}

JSpriteRenderer::~JSpriteRenderer() {
    // 清理 VBO
    glDeleteBuffers(1, &VBO);
}

void JSpriteRenderer::BindTexture(JTexture *tex, int textureFilter) {
    glBindTexture(GL_TEXTURE_2D, tex->mTexId);

    if (textureFilter == TEX_FILTER_LINEAR) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else if (textureFilter == TEX_FILTER_NEAREST) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void JSpriteRenderer::DrawSprite(JSprite &sprite) {
    this->shader.Use();

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 transform = glm::mat4(1.0f);

    if (sprite.scale.x != 1.0f || sprite.scale.y != 1.0f || sprite.rotate != 0.0f) {
        float cosTheta = cos(sprite.rotate);
        float sinTheta = sin(sprite.rotate);

        transform[0][0] = cosTheta * sprite.scale.x;
        transform[0][1] = sinTheta * sprite.scale.x;
        transform[1][0] = -sinTheta * sprite.scale.y;
        transform[1][1] = cosTheta * sprite.scale.y;
    }

    if (sprite.position != glm::vec2(0.0f, 0.0f)) {
        transform[3][0] = sprite.position.x;
        transform[3][1] = sprite.position.y;
    }

    model = transform;

    if (sprite.hotspot != glm::vec2(0.0f, 0.0f)) {
        model = glm::translate(model, glm::vec3(-sprite.hotspot, 0.0f));
    }

    if (sprite.spriteRect[2] > 0.0f && sprite.spriteRect[3] > 0.0f) {
        model = glm::scale(model, glm::vec3(sprite.spriteRect[2], sprite.spriteRect[3], 1.0f));
    }

    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(spriteRectLocation, sprite.spriteRect.x, sprite.spriteRect.y, sprite.spriteRect.z, sprite.spriteRect.w);
    glUniform2f(textureSizeLocation, sprite.texture->mTexWidth, sprite.texture->mTexHeight);
    glUniform2i(flippedLocation, sprite.hFlipped, sprite.vFlipped);
    glUniform4f(colorLocation, sprite.color.x, sprite.color.y, sprite.color.z, sprite.color.w);

    glActiveTexture(GL_TEXTURE0);
    BindTexture(sprite.texture, sprite.textureFilter);

    // 綁定 VAO 並繪製
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

void JSpriteRenderer::initRenderData() {
    // 頂點數據：4 個頂點，每個包含 x, y 座標
    GLfloat vertices[] = {
        // Position
        0.0f, 1.0f,  // 左上角
        0.0f, 0.0f,  // 左下角
        1.0f, 0.0f,  // 右下角
        1.0f, 1.0f   // 右上角
    };

    // 索引數據：2 個三角形，6 個索引
    GLuint indices[] = {
        0, 1, 2,  // 第 1 個三角形
        2, 3, 0   // 第 2 個三角形
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // 設置 VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 設置 EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 設置頂點屬性指針
    glEnableVertexAttribArray(0); // 位置屬性
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    // 解綁 VAO 和 VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


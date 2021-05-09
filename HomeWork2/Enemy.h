//
// Created by Kologermansky on 09.05.2021.
//

#include <algorithm>
#include <utility>
#include <vector>
#include <fstream>
#include <cmath>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef HOMEWORK_FIGURES_H
#define HOMEWORK_FIGURES_H

class Enemy {
public:
    Enemy() = default;

    void init(GLuint programID, GLuint vertexbuffer, GLuint uvbuffer, size_t vc);

    void setCenter(glm::vec3 c);

    void setTexture(GLuint texture);

    bool isClose(glm::vec3 center);

    void draw(const glm::mat4& MVP);

private:

    // center
    glm::vec3 center;
    GLuint rotateID = 0;

    // coords
    size_t vertex_count = 0;
    GLuint vertexbuffer = 0;
    GLuint uvbuffer = 0;
    GLfloat angle = 0.0f;
    constexpr static GLfloat step = 0.0002f;

    // shader ID
    GLuint programID = 0;
    GLuint MatrixID = 0;

    // texture IDs
    GLuint texture = 0;
    GLuint textureID = 0;
    constexpr const static auto pi = glm::pi<GLfloat>();
};

void Enemy::init(GLuint programID_, GLuint vertexbuffer_, GLuint uvbuffer_, size_t vc) {
    vertexbuffer = vertexbuffer_;
    uvbuffer = uvbuffer_;
    vertex_count = vc;
    programID = programID_;
    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");
    rotateID = glGetUniformLocation(programID, "rotate");
    textureID = glGetUniformLocation(programID, "myTextureSampler");
}

void Enemy::setCenter(glm::vec3 c) {
    center = c;
}

void Enemy::setTexture(GLuint texture_) {
    texture = texture_;
}

bool Enemy::isClose(glm::vec3 v) {
    glm::vec3 dist = v - center;
    GLfloat len = glm::sqrt(dist.x * dist.x + dist.y * dist.y + dist.z * dist.z);
    return (len <= 0.33);
}

void Enemy::draw(const glm::mat4& MVP) {

    if ((angle += step) > 2 * pi) {
        angle -= 2 * pi;
    }

    GLfloat scale = 1;
    if (center.x != 0 || center.y != 0 || center.z != 0) {
        scale = 1 / glm::sqrt(center.x * center.x + center.y * center.y + center.z * center.z);
    }

    const glm::mat4 rotateMat = glm::translate(glm::mat4(), center) *
            glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1)) *
            glm::scale(glm::mat4(), glm::vec3(scale, scale, scale));

    // Use our shader
    glUseProgram(programID);

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(rotateID, 1, GL_FALSE, &rotateMat[0][0]);

    // Enable attribute arrays
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(textureID, 0);

    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            nullptr             // array buffer offset
    );

    // 2nd attribute buffer : colors
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
            1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            nullptr             // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);

    // Disable attribute arrays
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

#endif //HOMEWORK_FIGURES_H

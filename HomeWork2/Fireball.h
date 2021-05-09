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

#ifndef HOMEWORK_FIREBALL_H
#define HOMEWORK_FIREBALL_H

class Fireball {
public:

    Fireball() = default;

    void init(GLuint programID, GLuint vertexbuffer, GLuint uvbuffer, size_t vc);

    void setPosition(glm::vec3 pos);

    void setDirection(glm::vec3 d);

    void setTexture(GLuint texture);

    glm::vec3 getCenter();

    bool draw(const glm::mat4& MVP);

private:

    glm::vec3 direction;
    glm::vec3 position;
    GLuint rotateID = 0;

    // coords
    size_t vertex_count = 0;
    GLuint vertexbuffer = 0;
    GLuint uvbuffer = 0;
    GLfloat angle = 0.0f;
    constexpr static GLfloat angleStep = 0.001f;
    GLfloat dist = 1.5f;
    constexpr static GLfloat distStep = 0.001f;

    // shader ID
    GLuint programID = 0;
    GLuint MatrixID = 0;

    // texture IDs
    GLuint texture = 0;
    GLuint textureID = 0;
    constexpr const static auto pi = glm::pi<GLfloat>();
};

void Fireball::init(GLuint programID_, GLuint vertexbuffer_, GLuint uvbuffer_, size_t vc) {
    vertexbuffer = vertexbuffer_;
    uvbuffer = uvbuffer_;
    vertex_count = vc;
    programID = programID_;
    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");
    rotateID = glGetUniformLocation(programID, "rotate");
    textureID = glGetUniformLocation(programID, "myTextureSampler");
}

void Fireball::setDirection(glm::vec3 d) {
    direction = d;
}

void Fireball::setPosition(glm::vec3 pos) {
    position = pos;
}

void Fireball::setTexture(GLuint texture_) {
    texture = texture_;
}

glm::vec3 Fireball::getCenter() {
    return direction * dist + position;
}

bool Fireball::draw(const glm::mat4& MVP) {

    if ((angle += angleStep) > 2 * pi) {
        angle -= 2 * pi;
    }

    dist += distStep;

    glm::vec3 c = getCenter();
    GLfloat scale = glm::sqrt(c.x * c.x + c.y * c.y + c.z * c.z);
    if (scale > 6) {
        return false;
    }

    const glm::mat4 rotateMat = glm::translate(glm::mat4(), c) *
                                glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1)) *
                                glm::scale(glm::mat4(), glm::vec3(1 / scale, 1 / scale, 1 / scale));

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

    return true;
}

#endif //HOMEWORK_FIREBALL_H

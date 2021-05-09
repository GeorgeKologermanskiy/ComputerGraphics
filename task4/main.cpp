//
// Created by Kologermansky on 12.03.2021.
//
// Include standard headers
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

// points
#define A -0.5f, -0.2f, 0.9f
#define B -0.2f, 0.5f, 0.9f
#define C 0.7f, 0.1f, 0.9f
#define D -0.5f, -0.2f, -0.9f
#define E -0.2f, 0.5f, -0.9f
#define F 0.7f, 0.1f, -0.9f
// colors
#define A_c 0.2f, 0.3f, 0.7f, 0.5f
#define B_c 0.3f, 0.5f, 0.1f, 0.9f
#define C_c 0.7f, 0.1f, 0.0f, 0.2f
#define D_c 0.1f, 0.8f, 0.3f, 0.1f
#define E_c 0.6f, 0.1f, 0.3f, 0.4f
#define F_c 0.7f, 0.1f, 0.9f, 0.1f

int main() {
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow( 1024, 768, "Task 4", NULL, NULL);
    if(nullptr == window) {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0.7f, 0.8f, 0.9f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("EnemyVertexShader.shader", "EnemyFragmentShader.shader");

    // generate vertex buffer
    GLuint vertexbuffer;
    static const GLfloat g_vertex_buffer_data[] = {
            // high triangle
            A, B, C,
            // back
            A, B, D,
            D, B, E,
            // left
            A, C, D,
            D, C, F,
            // right
            C, B, F,
            B, F, E,
            // low triangle
            D, E, F
    };
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


    // generate color buffer
    GLuint colorbuffer;
    static const GLfloat g_color_buffer_data[] = {
            // high
            A_c, B_c, C_c,
            // back
            A_c, B_c, D_c,
            D_c, B_c, E_c,
            // left
            A_c, C_c, D_c,
            D_c, C_c, F_c,
            // right
            C_c, B_c, F_c,
            B_c, F_c, E_c,
            // low
            D_c, E_c, F_c,
    };
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");


    GLfloat alpha = 0.0f;
    const GLfloat step = 0.0005f;
    const auto PI = glm::pi<GLfloat>();
    const glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // step
        if ((alpha += step) > 2 * PI) {
            alpha -= 2 * PI;
        }
        // Camera matrix
        glm::mat4 View = glm::lookAt(
                glm::vec3(3 * glm::sin(alpha),0,3 * glm::cos(alpha)),
                glm::vec3(0,0,0),
                glm::vec3(0,1,0)
        );

        glm::mat4 MVP = Projection * View;// * Model;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Enable attribute arrays
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // 1rst attribute buffer : vertices
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                NULL            // array buffer offset
        );

        // 2nd attribute buffer : colors
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
                1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
                4,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                NULL            // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3 * 8);

        // Disable attribute arrays
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    glfwTerminate();
    return 0;
}

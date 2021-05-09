//
// Created by Kologermansky on 12.03.2021.
//
// Include standard headers
#include <cstdio>
#include <random>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

GLFWwindow *window;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>

#include "Enemy.h"
#include "Fireball.h"

// vertex array ID
GLuint VertexArrayID;

// shaders ID
GLuint enemyProgramID;
GLuint fireballProgramID;

// texture IDs
GLuint flameTexture;
GLuint brickTexture;

std::vector<GLfloat> vertex_sphere_buffer;
std::vector<GLfloat> vertex_uv_sphere_buffer;
GLuint sphere_vertexbuffer;
GLuint sphere_uvbuffer;
size_t sphere_vertex_count;

std::vector<GLfloat> vertex_pyramid_buffer;
std::vector<GLfloat> vertex_uv_pyramid_buffer;
GLuint pyramid_vertexbuffer;
GLuint pyramid_uvbuffer;
size_t pyramid_vertex_count;

void getFigureCoords(const std::string &path, std::vector<GLfloat> &res_v, std::vector<GLfloat> &res_uv) {
    std::ifstream ifs;
    ifs.open(path);

    int vertexes_count;
    std::string s;
    ifs >> vertexes_count >> s;
    std::vector<GLfloat> vertexes(3 * vertexes_count);
    std::vector<GLfloat> uv(2 * vertexes_count);

    const auto pi = glm::pi<GLfloat>();

    for (int i = 0; i < vertexes_count; ++i) {
        ifs >> vertexes[3 * i] >> vertexes[3 * i + 1] >> vertexes[3 * i + 2];
        GLfloat x = vertexes[3 * i];
        GLfloat y = vertexes[3 * i + 1];
        GLfloat z = vertexes[3 * i + 2];
        GLfloat p = std::sqrt(x * x + y * y + z * z);
        if (p > 1e-9) {
            x /= p;
            y /= p;
            z /= p;
        }
        uv[2 * i + 0] = 0.5 + std::atan2(x, z) / (2 * pi);
        uv[2 * i + 1] = 0.5 - std::asin(y) / pi;
    }

    int facets_count;
    ifs >> facets_count >> s;
    res_v.resize(9 * facets_count);
    res_uv.resize(6 * facets_count);
    for (int i = 0; i < facets_count; ++i) {
        int a, b, c;
        ifs >> a >> b >> c;

        res_v[9 * i + 0] = vertexes[3 * a + 0];
        res_v[9 * i + 1] = vertexes[3 * a + 1];
        res_v[9 * i + 2] = vertexes[3 * a + 2];
        res_v[9 * i + 3] = vertexes[3 * b + 0];
        res_v[9 * i + 4] = vertexes[3 * b + 1];
        res_v[9 * i + 5] = vertexes[3 * b + 2];
        res_v[9 * i + 6] = vertexes[3 * c + 0];
        res_v[9 * i + 7] = vertexes[3 * c + 1];
        res_v[9 * i + 8] = vertexes[3 * c + 2];

        res_uv[6 * i + 0] = uv[2 * a + 0];
        res_uv[6 * i + 1] = uv[2 * a + 1];
        res_uv[6 * i + 2] = uv[2 * b + 0];
        res_uv[6 * i + 3] = uv[2 * b + 1];
        res_uv[6 * i + 4] = uv[2 * c + 0];
        res_uv[6 * i + 5] = uv[2 * c + 1];
    }
}

bool loadGame() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 768, "Simple Game", nullptr, nullptr);
    if (nullptr == window) {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        getchar();
        glfwTerminate();
        return false;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glClearColor(0.7f, 0.8f, 0.9f, 0.0f);

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    enemyProgramID = LoadShaders("shaders/EnemyVertexShader.shader",
                                 "shaders/EnemyFragmentShader.shader");
    fireballProgramID = LoadShaders("shaders/FireballVertexShader.shader",
                                    "shaders/FireballFragmentShader.shader");

    // load textures
    flameTexture = loadBMP_custom("pictures/flame.bmp");
    brickTexture = loadBMP_custom("pictures/brick.bmp");

    // read vertex coordinates
    getFigureCoords("pictures/sphere", vertex_sphere_buffer, vertex_uv_sphere_buffer);
    sphere_vertex_count = vertex_sphere_buffer.size();
    glGenBuffers(1, &sphere_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_sphere_buffer.size() * sizeof(GLfloat), &vertex_sphere_buffer[0],
                 GL_STATIC_DRAW);
    glGenBuffers(1, &sphere_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_uv_sphere_buffer.size() * sizeof(GLfloat), &vertex_uv_sphere_buffer[0],
                 GL_STATIC_DRAW);

    getFigureCoords("pictures/pyramid", vertex_pyramid_buffer, vertex_uv_pyramid_buffer);
    pyramid_vertex_count = vertex_pyramid_buffer.size();
    glGenBuffers(1, &pyramid_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, pyramid_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_pyramid_buffer.size() * sizeof(GLfloat), &vertex_pyramid_buffer[0],
                 GL_STATIC_DRAW);
    glGenBuffers(1, &pyramid_uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, pyramid_uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_uv_pyramid_buffer.size() * sizeof(GLfloat), &vertex_uv_pyramid_buffer[0],
                 GL_STATIC_DRAW);

    return true;
}

void close() {
    // Cleanup VBO
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(enemyProgramID);
    glDeleteProgram(fireballProgramID);

    glDeleteBuffers(1, &sphere_vertexbuffer);
    glDeleteBuffers(1, &sphere_uvbuffer);
    glDeleteBuffers(1, &pyramid_vertexbuffer);
    glDeleteBuffers(1, &pyramid_uvbuffer);


    glfwTerminate();

}

int main() {
    if (!loadGame()) {
        return -1;
    }

    std::vector<Enemy> enemies;
    std::vector<Fireball> fireballs;


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    const glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024.0 / 2, 768.0 / 2);

    int mouseState = GLFW_RELEASE;

    double last_add_time = glfwGetTime();

    std::random_device rd;
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> len_distr(2.0, 6.0);
    std::uniform_real_distribution<> alpha_distr(0.0, 2 * pi<float>());

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        double curr_time = glfwGetTime();

        if (curr_time - last_add_time > 1 && enemies.size() < 16) {
            enemies.emplace_back();
            auto &e = enemies.back();
            e.init(enemyProgramID, pyramid_vertexbuffer, pyramid_uvbuffer, pyramid_vertex_count);

            const double r = len_distr(gen);
            const double phi = alpha_distr(gen);
            const double psi = alpha_distr(gen);
            glm::vec3 c = glm::vec3(
                    cos(phi) * sin(psi) * r,
                    sin(phi) * r,
                    cos(phi) * cos(psi) * r
            );

            e.setCenter(c);
            e.setTexture(brickTexture);
            last_add_time = curr_time;
        }


        int currMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (mouseState == GLFW_RELEASE && currMouseState == GLFW_PRESS) {
            fireballs.emplace_back();
            auto &f = fireballs.back();
            f.init(fireballProgramID, sphere_vertexbuffer, sphere_uvbuffer, sphere_vertex_count);
            f.setPosition(getPosition());
            f.setDirection(getDirection());
            f.setTexture(flameTexture);
        }
        mouseState = currMouseState;

        for (size_t i = 0; i < fireballs.size();) {
            bool connected = false;
            const glm::vec3 c = fireballs[i].getCenter();
            for (size_t j = 0; j < enemies.size(); ++j) {
                if (enemies[j].isClose(c)) {
                    connected = true;
                    enemies.erase(enemies.begin() + j);
                    break;
                }
            }
            if (connected) {
                fireballs.erase(fireballs.begin() + i);
            } else {
                ++i;
            }
        }


        for (auto &e : enemies) {
            e.draw(MVP);
        }

        for (size_t i = 0; i < fireballs.size();) {
            if (fireballs[i].draw(MVP)) {
                ++i;
                continue;
            } else {
                fireballs.erase(fireballs.begin() + i);
            }
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    close();

    return 0;
}

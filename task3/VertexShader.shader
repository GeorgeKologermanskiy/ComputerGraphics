#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;

out vec4 color;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main() {
    gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);

    color = vertexColor;
}

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

out vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 rotate;

void main() {
    gl_Position =  MVP * rotate * vec4(vertexPosition_modelspace, 1);
    UV = vertexUV;
}

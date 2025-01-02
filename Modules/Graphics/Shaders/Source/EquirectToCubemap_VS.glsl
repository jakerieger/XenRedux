#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 LocalPos;

uniform mat4 uProjection;
uniform mat4 uView;

void main() {
    LocalPos = aPos;
    gl_Position = uProjection * uView * vec4(LocalPos, 1.0);
}
#version 460 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 uVP;
uniform mat4 uModel;

out vec2 TexCoords;

void main() {
    vec4 pos = uVP * uModel * vec4(aPosition.x, aPosition.y, 0.0, 1.0);
    gl_Position = pos;
    TexCoords = aTexCoords;
}
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uVP;
uniform mat4 uModel;

out vec4 VertexPosition;
out vec2 TexCoord;

void main() {
    vec4 pos = vec4(aPos, 1.0);
    gl_Position = uVP * uModel * pos;
    VertexPosition = pos;
    TexCoord = aTexCoord;
}
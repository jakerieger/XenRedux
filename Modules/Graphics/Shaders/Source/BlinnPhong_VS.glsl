#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uVP;
uniform mat4 uModel;

out vec4 VertexPosition;
out vec2 TexCoord;

void main() {
    vec4 pos = vec4(aPos, 1.0);
    mat4 mvp = uVP * uModel * pos;
    gl_Position = pos;
    VertexPosition = pos;
    TexCoord = aTexCoord;
}
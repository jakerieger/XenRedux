#version 460 core

in vec4 VertexPosition;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec2 TexCoord;

out vec4 FragColor;

void main() {
    FragColor = vec4(Normal, 1.0);
}
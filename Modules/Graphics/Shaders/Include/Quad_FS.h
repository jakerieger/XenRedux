#pragma once
static const char* Quad_FS_Source = R""(
#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uRenderTexture;

void main() {
    vec4 color = texture(uRenderTexture, TexCoords);
    //    color = 1.0 - color + 0.5;
    FragColor = color;
}
)"";

#pragma once
static const char* Unlit_FS_Source = R""(
#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uTexture;

void main() {
    vec4 color = texture(uTexture, TexCoords);
    FragColor = color; //vec4(1.0, 0.0, 0.0, 1.0);
}
)"";

#pragma once
static const char* Skybox_FS_Source = R""(
#version 460 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube uSkybox;

void main() {
    FragColor = texture(uSkybox, TexCoords);
}
)"";

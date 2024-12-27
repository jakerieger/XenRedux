#pragma once
static const char* Skybox_VS_Source = R""(
#version 460 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 uVP;

void main() {
    TexCoords = aPos;
    vec4 pos = uVP * vec4(aPos, 1.0);
    gl_Position = pos.xyww;// set z to w for depth
}
)"";

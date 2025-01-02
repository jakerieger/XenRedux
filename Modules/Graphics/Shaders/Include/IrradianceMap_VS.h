#pragma once
static const char* IrradianceMap_VS_Source = R""(
#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 WorldPos;

uniform mat4 uProjection;
uniform mat4 uView;

void main() {
    WorldPos = aPos;
    mat4 rotView = mat4(mat3(uView));
    vec4 clipPos = uProjection * rotView * vec4(WorldPos, 1.0);
    gl_Position = clipPos;
}
)"";

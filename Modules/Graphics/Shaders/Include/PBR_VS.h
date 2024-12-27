#pragma once
static const char* PBR_VS_Source = R""(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBiTangent;
layout (location = 4) in vec2 aTexCoord;

uniform mat4 uVP;
uniform mat4 uModel;

out struct VSOut {
    vec2 texCoord;
    vec3 fragPos;
    vec3 normal;
} vsOut;

void main() {
    vsOut.fragPos = vec3(uModel * vec4(aPos, 1.0));
    mat3 normalMatrix = mat3(transpose(inverse(uModel)));
    vsOut.normal = normalize(normalMatrix * aNormal);
    vsOut.texCoord = aTexCoord;
    gl_Position = uVP * vec4(vsOut.fragPos, 1.0);
}
)"";

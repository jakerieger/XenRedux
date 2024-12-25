#pragma once
static const char* BlinnPhong_VS_Source = R""(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBiTangent;
layout (location = 4) in vec2 aTexCoord;

uniform mat4 uVP;
uniform mat4 uModel;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 FragNormal;

void main() {
    FragPos = vec3(uModel * vec4(aPos, 1.0));

    mat3 normalMatrix = mat3(transpose(inverse(uModel)));
    FragNormal = normalize(normalMatrix * aNormal);

    TexCoord = aTexCoord;

    gl_Position = uVP * vec4(FragPos, 1.0);
}
)"";

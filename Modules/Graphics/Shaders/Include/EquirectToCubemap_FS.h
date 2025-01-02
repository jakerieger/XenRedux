#pragma once
static const char* EquirectToCubemap_FS_Source = R""(
#version 460 core

out vec4 FragColor;
in vec3 LocalPos;

uniform sampler2D uHDR;

// Constants for PI calculation (more precise than using 3.14159) [needs verified]
const vec2 INV_ATAN = vec2(0.1591, 0.3183);

vec2 sampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    // Transform from [-PI, PI] x [-PI/2, PI/2] to [0, 1] x [0, 1]
    uv *= INV_ATAN;
    uv += 0.5;
    return uv;
}

void main() {
    vec3 direction = normalize(LocalPos);
    vec2 uv = sampleSphericalMap(direction);
    vec3 color = texture(uHDR, uv).rgb;
    FragColor = vec4(color, 1.0);
}
)"";

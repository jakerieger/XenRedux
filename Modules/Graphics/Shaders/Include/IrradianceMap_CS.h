#pragma once
static const char* IrradianceMap_CS_Source = R""(
#version 460
layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(binding = 0) uniform samplerCube uEnvMap;
layout(rgba16f, binding = 1) uniform imageCube uIrrMap;

layout(std140, binding = 0) uniform IrradianceParams {
    int face;
    float deltaPhi;
    float deltaTheta;
    int padding;
};

const float PI = 3.14159265359;

// Convert cube face texel coordinate to direction vector
vec3 getFaceDirection(ivec2 coord, int face, ivec2 dim) {
    vec2 uv = (vec2(coord) + 0.5) / vec2(dim) * 2.0 - 1.0;

    switch (face) {
        case 0: return normalize(vec3(1.0, -uv.y, -uv.x));// POS_X
        case 1: return normalize(vec3(-1.0, -uv.y, uv.x));// NEG_X
        case 2: return normalize(vec3(uv.x, 1.0, uv.y));// POS_Y
        case 3: return normalize(vec3(uv.x, -1.0, -uv.y));// NEG_Y
        case 4: return normalize(vec3(uv.x, -uv.y, 1.0));// POS_Z
        case 5: return normalize(vec3(-uv.x, -uv.y, -1.0));// NEG_Z
        default : return vec3(0.0);
    }
}

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dimensions = imageSize(uIrrMap);

    if (pixel_coords.x >= dimensions.x || pixel_coords.y >= dimensions.y) {
        return;
    }

    vec3 N = getFaceDirection(pixel_coords, face, dimensions);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    vec3 irradiance = vec3(0.0);
    float nrSamples = 0.0;

    for (float phi = 0.0; phi < 2.0 * PI; phi += deltaPhi) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += deltaTheta) {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

            irradiance += texture(uEnvMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / nrSamples);
    //    imageStore(uIrrMap, ivec3(pixel_coords, face), vec4(irradiance, 1.0));
    imageStore(uIrrMap, ivec3(pixel_coords, face), vec4(1.0, 0.0, 0.0, 1.0));
}
)"";

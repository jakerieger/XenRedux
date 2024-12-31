#pragma once
static const char* PrefilterMap_CS_Source = R""(
#version 460
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

// Input environment cubemap
layout(binding = 0) uniform samplerCube uEnvironmentMap;

// Output prefiltered environment map face
layout(binding = 1, rgba16f) uniform writeonly image2D uPrefiltered;

// Parameters for prefiltering
layout(std140, binding = 2) uniform PrefilterParams {
    mat4 viewMatrix;// View matrix for current face
    int faceIndex;// Current face being processed
    float roughness;// Current roughness level
    int numSamples;// Number of samples to take
    float resolution;// Resolution of the current mip level
};

// Constants
const float PI = 3.14159265359;
const float TWO_PI = 2.0 * PI;

// Hammersley sequence for generating sample points
vec2 hammersley(uint i, uint N) {
    uint bits = (i << 16u) | (i >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    float radicalInverse = float(bits) * 2.3283064365386963e-10;
    return vec2(float(i) / float(N), radicalInverse);
}

// Importance sampling GGX distribution
vec3 importanceSampleGGX(vec2 Xi, float roughness, vec3 N) {
    float a = roughness * roughness;
    float phi = TWO_PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // Convert to cartesian coordinates
    vec3 H = vec3(
    sinTheta * cos(phi),
    sinTheta * sin(phi),
    cosTheta
    );

    // Convert from tangent to world space
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangentX = normalize(cross(up, N));
    vec3 tangentY = cross(N, tangentX);

    return tangentX * H.x + tangentY * H.y + N * H.z;
}

// Convert UV coordinates to direction vector
vec3 uvToXYZ(ivec2 texCoord, int size) {
    vec2 uv = (vec2(texCoord) + 0.5) / float(size) * 2.0 - 1.0;

    vec3 direction;
    switch (faceIndex) {
        case 0: direction = vec3(1.0, -uv.y, -uv.x); break;
        case 1: direction = vec3(-1.0, -uv.y, uv.x); break;
        case 2: direction = vec3(uv.x, 1.0, uv.y); break;
        case 3: direction = vec3(uv.x, -1.0, -uv.y); break;
        case 4: direction = vec3(uv.x, -uv.y, 1.0); break;
        case 5: direction = vec3(-uv.x, -uv.y, -1.0); break;
    }
    return normalize(direction);
}

void main() {
    ivec2 texCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(uPrefiltered);

    if (any(greaterThanEqual(texCoord, size))) return;

    // Get direction for current texel
    vec3 N = uvToXYZ(texCoord, size.x);
    vec3 R = N;
    vec3 V = R;

    // Accumulate filtered color
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    for (uint i = 0; i < numSamples; i++) {
        vec2 Xi = hammersley(i, numSamples);
        vec3 H = importanceSampleGGX(Xi, roughness, N);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);

        if (NdotL > 0.0) {
            // Sample environment map
            vec3 sampleColor = texture(uEnvironmentMap, L).rgb;
            prefilteredColor += sampleColor * NdotL;
            totalWeight += NdotL;
        }
    }

    // Normalize
    prefilteredColor = prefilteredColor / totalWeight;

    // Store result
    imageStore(uPrefiltered, texCoord, vec4(prefilteredColor, 1.0));
}
)"";

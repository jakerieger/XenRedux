#pragma once
static const char* PrefilteredMap_FS_Source = R""(
#version 460 core

out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube uEnvironmentMap;
uniform float uRoughness;

const float PI = 3.14159265359;


// Hammersley sequence for quasi-Monte Carlo sampling
float radicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;// what the fuck, man
}

// Generate a point in sampling space
vec2 hammersley(uint i, uint N) {
    return vec2(float(i)/float(N), radicalInverse_VdC(i));
}

// Importance sampling using GGX distribution
vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    // Map our random numbers to spherical coordinates
    float a = roughness * roughness;
    float phi = 2.0 * PI * Xi.x;

    // Use roughness to determine how spread out our samples should be
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // Convert to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // Create TBN matrix for transforming from tangent to world space
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    // Transform sample from tangent to world space
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

// Distribution function for GGX/Trowbridge-Reitz
float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

void main() {
    vec3 N = normalize(WorldPos);
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    // Debug: Let's verify our sample distribution
    float solidAngleSum = 0.0;

    for (uint i = 0u; i < SAMPLE_COUNT; i++) {
        vec2 Xi = hammersley(i, SAMPLE_COUNT);
        vec3 H = importanceSampleGGX(Xi, N, uRoughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);

        if (NdotL > 0.0) {
            // For rougher surfaces, we should be gathering light from a wider area
            float weight = NdotL;

            // The key is ensuring our sampling pattern spreads out more with roughness
            vec3 sampleColor = textureLod(uEnvironmentMap, L, 0.0).rgb;
            prefilteredColor += sampleColor * weight;
            totalWeight += weight;

            // Debug: Accumulate the solid angle we're sampling from
            solidAngleSum += acos(NdotL);
        }
    }

    if (totalWeight > 0.0) {
        prefilteredColor = prefilteredColor / totalWeight;
    }

    // Debug: Visualize our sampling spread
    float samplingSpread = solidAngleSum / float(SAMPLE_COUNT);
    FragColor = vec4(vec3(samplingSpread), 1.0);
    return;

    FragColor = vec4(prefilteredColor, 1.0);
}
)"";

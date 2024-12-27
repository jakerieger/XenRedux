#pragma once
static const char* FXAA_CS_Source = R""(
#version 460

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

uniform sampler2D uInputTexture;
layout(rgba16f, binding = 0) uniform writeonly image2D uOutputTexture;

// Constants
const float EDGE_THRESHOLD_MIN = 0.0312;
const float EDGE_THRESHOLD_MAX = 0.125;
const float SUBPIXEL_QUALITY = 0.25;
const int ITERATIONS = 6;

float GetLuminance(vec3 color) {
    return sqrt(dot(color, vec3(0.299, 0.587, 0.114)));
}

void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 texelSize = 1.0 / textureSize(uInputTexture, 0);
    vec2 uv = (vec2(pixelCoords) + 0.5) * texelSize;// Center of pixel

    // Sample 3x3 neighborhood
    vec3 rgbNW = textureOffset(uInputTexture, uv, ivec2(-1, -1)).rgb;
    vec3 rgbNE = textureOffset(uInputTexture, uv, ivec2(1, -1)).rgb;
    vec3 rgbSW = textureOffset(uInputTexture, uv, ivec2(-1, 1)).rgb;
    vec3 rgbSE = textureOffset(uInputTexture, uv, ivec2(1, 1)).rgb;
    vec3 rgbM  = texture(uInputTexture, uv).rgb;

    // Convert to luminance
    float lumaNW = GetLuminance(rgbNW);
    float lumaNE = GetLuminance(rgbNE);
    float lumaSW = GetLuminance(rgbSW);
    float lumaSE = GetLuminance(rgbSE);
    float lumaM  = GetLuminance(rgbM);

    // Find the maximum and minimum luminance
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    float lumaRange = lumaMax - lumaMin;

    // If contrast is lower than threshold, skip processing
    if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX)) {
        imageStore(uOutputTexture, pixelCoords, vec4(rgbM, 1.0));
        return;
    }

    // Sample additional pixels for edge detection
    vec3 rgbL = textureOffset(uInputTexture, uv, ivec2(-1, 0)).rgb;
    vec3 rgbR = textureOffset(uInputTexture, uv, ivec2(1, 0)).rgb;
    vec3 rgbT = textureOffset(uInputTexture, uv, ivec2(0, -1)).rgb;
    vec3 rgbB = textureOffset(uInputTexture, uv, ivec2(0, 1)).rgb;

    float lumaL = GetLuminance(rgbL);
    float lumaR = GetLuminance(rgbR);
    float lumaT = GetLuminance(rgbT);
    float lumaB = GetLuminance(rgbB);

    // Calculate gradients and determine edge direction
    float lumaGradH = abs((lumaNW + lumaNE) - (2.0 * lumaM)) * 2.0 +
    abs(lumaT - lumaB);
    float lumaGradV = abs((lumaNW + lumaSW) - (2.0 * lumaM)) * 2.0 +
    abs(lumaL - lumaR);

    // Determine edge direction (true = horizontal)
    bool isHorizontal = lumaGradH >= lumaGradV;

    // Choose step size based on edge direction
    vec2 stepSize = isHorizontal ? vec2(texelSize.x, 0.0) : vec2(0.0, texelSize.y);
    float gradientStep = isHorizontal ? lumaGradH : lumaGradV;
    gradientStep = min(gradientStep, 1.0);

    // Determine positive or negative direction
    float lumaP1 = isHorizontal ? lumaT : lumaL;
    float lumaP2 = isHorizontal ? lumaB : lumaR;
    bool is1Steeper = abs(lumaP1 - lumaM) >= abs(lumaP2 - lumaM);
    float stepDir = is1Steeper ? (lumaP1 > lumaM ? 1.0 : -1.0) :
    (lumaP2 > lumaM ? 1.0 : -1.0);

    // Sample along the edge
    vec3 rgbResult = rgbM;
    vec2 uv1 = uv - stepSize * stepDir;
    vec2 uv2 = uv + stepSize * stepDir;

    for (int i = 0; i < ITERATIONS; i++) {
        float weight = 1.0 - (float(i) / float(ITERATIONS));
        vec3 rgbS1 = texture(uInputTexture, uv1).rgb;
        vec3 rgbS2 = texture(uInputTexture, uv2).rgb;

        rgbResult = mix(rgbResult, (rgbS1 + rgbS2) * 0.5,
        weight * SUBPIXEL_QUALITY);

        uv1 -= stepSize * stepDir;
        uv2 += stepSize * stepDir;
    }

    imageStore(uOutputTexture, pixelCoords, vec4(rgbResult, 1.0));
}
)"";

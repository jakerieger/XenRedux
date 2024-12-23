#pragma once
static const char* Gaussian_CS_Source = R""(
#version 460

layout (local_size_x = 16, local_size_y = 16) in;

uniform sampler2D uInputTexture;
uniform ivec2 uTextureSize; // The size of the texture in pixels (width, height)
uniform float uKernel[5]; // Gaussian kernel

layout (binding = 0) writeonly uniform image2D uOutputTexture;

vec4 verticalPass(ivec2 texelCoord) {
    // Sample the input texture in the vertical direction
    vec4 color = vec4(0.0);
    for (int i = -2; i <= 2; i++) {
        vec2 offset = vec2(0, i);
        color += texture(uInputTexture, (texelCoord + offset) / vec2(uTextureSize)) * uKernel[i + 2];
    }
    return color;
}

vec4 horizontalPass(ivec2 texelCoord) {
    // Sample the input texture in the horizontal direction
    vec4 color = vec4(0.0);
    for (int i = -2; i <= 2; i++) {
        vec2 offset = vec2(i, 0);
        color += texture(uInputTexture, (texelCoord + offset) / vec2(uTextureSize)) * uKernel[i + 2];
    }
    return color;
}

void main() {
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    vec4 vertical = verticalPass(texelCoord);
    vec4 horizontal = horizontalPass(texelCoord);
    vec4 color = mix(vertical, horizontal, 0.5); // I have no idea if this is right but we'll see

    imageStore(uOutputTexture, texelCoord, color);
}
)"";

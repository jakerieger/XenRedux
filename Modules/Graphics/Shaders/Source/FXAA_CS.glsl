#version 460

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// Bind our HDR color buffer as an image
layout(rgba16f, binding = 0) uniform readonly image2D uInputTexture;
// Output buffer (back buffer format)
layout(rgba16f, binding = 1) uniform writeonly image2D uOutputTexture;

// Constants
const float EDGE_THRESHOLD_MIN = 0.0312;
const float EDGE_THRESHOLD_MAX = 0.125;
const float SUBPIXEL_QUALITY = 0.75;
const int ITERATIONS = 12;

float GetLuminance(vec3 color) {
    return dot(color, vec3(0.299, 0.587, 0.114));
}

void main() {
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 texelSize = 1.0 / textureSize(uInputTexture, 0);
    vec2 uv = (vec2(pixelCoords) + 0.5) * texelSize;
}
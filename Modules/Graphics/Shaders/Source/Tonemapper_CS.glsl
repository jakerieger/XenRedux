#version 460

// Define the workgroup size - we'll process pixels in 8x8 blocks
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Bind our HDR color buffer as an image
layout(rgba16f, binding = 0) uniform readonly image2D hdrBuffer;
// Output buffer (back buffer format)
layout(rgba8, binding = 1) uniform writeonly image2D outputBuffer;

// Tonemapping parameters
layout(std140, binding = 0) uniform TonemapParams {
    float exposure;
    float gamma;
    int tonemapOperator;// 0 = ACES, 1 = Reinhard, 2 = Exposure
    int padding;
};

// ACES tonemapping curve fit
vec3 ACESFilm(vec3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

// Reinhard tonemapping
vec3 Reinhard(vec3 x) {
    return x / (x + vec3(1.0));
}

// Exposure tonemapping
vec3 ExposureTonemap(vec3 x, float exposure) {
    return vec3(1.0) - exp(-x * exposure);
}

void main() {
    // Get the pixel coordinates we're working on
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);

    // Read the HDR color value
    vec4 hdrColor = imageLoad(hdrBuffer, pixelCoord);
    vec3 color = hdrColor.rgb;

    // Apply tonemapping based on selected operator
    switch (tonemapOperator) {
        case 0:
        color = ACESFilm(color * exposure);
        break;
        case 1:
        color = Reinhard(color * exposure);
        break;
        case 2:
        color = ExposureTonemap(color, exposure);
        break;
    }

    // Apply gamma correction
    color = pow(color, vec3(1.0 / gamma));

    // Store the result
    imageStore(outputBuffer, pixelCoord, vec4(color, hdrColor.a));
}
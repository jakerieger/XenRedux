#version 460

layout (local_size_x = 16, local_size_y = 16) in;

uniform sampler2D uInputTexture;
uniform ivec2 uTextureSize; // Texture size in pixels
uniform float uKernel[9]; // Gaussian kernel
uniform float uBlurStrength;

layout (binding = 0) writeonly uniform image2D uOutputTexture;

void main() {
    ivec2 globalID = ivec2(gl_GlobalInvocationID.xy); // Get pixel position

    // Ensure we don't process pixels outside the texture
    if (globalID.x >= uTextureSize.x || globalID.y >= uTextureSize.y) return;

    // To store the final color after applying the kernel
    vec4 color = vec4(0.0);

    // Horizontal blur (in x-direction)
    for (int i = -4; i <= 4; ++i) { // Adjusted to a 9-tap kernel
                                    ivec2 texCoord = globalID + ivec2(i, 0); // Get neighboring pixel in x-direction
                                    texCoord = clamp(texCoord, ivec2(0), uTextureSize - ivec2(1)); // Clamp to valid texture coordinates
                                    color += texture(uInputTexture, texCoord / vec2(uTextureSize)) * uKernel[i + 4] * uBlurStrength; // Apply kernel weight and blur strength
    }

    // Vertical blur (in y-direction)
    vec4 finalColor = vec4(0.0);
    for (int i = -4; i <= 4; ++i) { // Adjusted to a 9-tap kernel
                                    ivec2 texCoord = globalID + ivec2(0, i); // Get neighboring pixel in y-direction
                                    texCoord = clamp(texCoord, ivec2(0), uTextureSize - ivec2(1)); // Clamp to valid texture coordinates
                                    finalColor += texture(uInputTexture, texCoord / vec2(uTextureSize)) * uKernel[i + 4] * uBlurStrength; // Apply kernel weight and blur strength
    }

    // Write the final color to the output texture
    imageStore(uOutputTexture, globalID, finalColor);
}

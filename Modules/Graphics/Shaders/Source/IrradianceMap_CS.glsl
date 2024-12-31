#version 460
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(binding = 0) uniform samplerCube uEnvironmentMap;
layout(binding = 1, rgba16f) uniform writeonly image2D uIrradiance;

layout(std140, binding = 2) uniform IrradianceParams {
    mat4 viewMatrix;// View matrix for the current face
    int faceIndex;// Current face being processed
    float deltaPhi;// Sampling step size for phi angle
    float deltaTheta;// Sampling step size for theta angle
    int numSamples;// Total number of samples to take
};

const float PI = 3.14159265359;
const float TWO_PI = 2.0 * PI;
const float HALF_PI = PI / 2.0;

// Convert UV coordinates and face index to a direction vector
vec3 uvToXYZ(ivec2 texCoord, int size) {
    // Convert texel coordinates to [-1, 1] range
    vec2 uv = (vec2(texCoord) + 0.5) / float(size) * 2.0 - 1.0;

    // Generate direction based on face index
    vec3 direction;
    switch (faceIndex) {
        case 0: direction = vec3(1.0, -uv.y, -uv.x); break;// Positive X
        case 1: direction = vec3(-1.0, -uv.y, uv.x); break;// Negative X
        case 2: direction = vec3(uv.x, 1.0, uv.y); break;// Positive Y
        case 3: direction = vec3(uv.x, -1.0, -uv.y); break;// Negative Y
        case 4: direction = vec3(uv.x, -uv.y, 1.0); break;// Positive Z
        case 5: direction = vec3(-uv.x, -uv.y, -1.0); break;// Negative Z
    }
    return normalize(direction);
}

// Convert tangent space vector to world space
vec3 tangentToWorld(vec3 tangentVec, vec3 N) {
    // Create tangent space basis vectors
    vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    // Transform the vector from tangent to world space
    return tangentVec.x * tangent + tangentVec.y * bitangent + tangentVec.z * N;
}

void main() {
    // Get current texel coordinate
    ivec2 texCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(uIrradiance);

    // Skip if outside the texture
    if (any(greaterThanEqual(texCoord, size))) return;

    // Get direction for current texel
    vec3 N = uvToXYZ(texCoord, size.x);

    // Initialize irradiance accumulation
    vec3 irradiance = vec3(0.0);
    float numSamplesTaken = 0.0;

    // Sample hemisphere around N
    for (float phi = 0.0; phi < TWO_PI; phi += deltaPhi) {
        for (float theta = 0.0; theta < HALF_PI; theta += deltaTheta) {
            // Convert spherical coordinates to cartesian in tangent space
            vec3 tangentSample = vec3(
            sin(theta) * cos(phi),
            sin(theta) * sin(phi),
            cos(theta)
            );

            // Convert to world space
            vec3 sampleVec = tangentToWorld(tangentSample, N);

            // Sample environment map
            vec3 sampleColor = texture(uEnvironmentMap, sampleVec).rgb;

            // Add weighted sample
            float weight = cos(theta) * sin(theta);
            irradiance += sampleColor * weight;
            numSamplesTaken += weight;
        }
    }

    // Normalize and apply PI factor
    irradiance = PI * irradiance / numSamplesTaken;

    // Store result
    imageStore(uIrradiance, texCoord, vec4(irradiance, 1.0));
}
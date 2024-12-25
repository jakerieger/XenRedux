#version 460 core

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

struct Sun {
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform Sun uSun;
uniform vec3 uViewPosition;
uniform Material uMaterial;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 FragNormal;

out vec4 FragColor;

const float PI = 3.14159265359;

// Calculate normal distribution function (NDF) using GGX/Trowbridge-Reitz
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

// Calculate geometry function using Smith's Schlick-GGX
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Calculate Fresnel equation using Fresnel-Schlick approximation
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    vec3 N = normalize(FragNormal);
    vec3 V = normalize(uViewPosition - FragPos);
    float roughness = max(0.05, uMaterial.roughness);

    // Calculate base reflectivity (F0)
    // Dialectrics (non-metals) have F0 of 0.04
    // Metals use their albedo as F0
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, uMaterial.albedo, uMaterial.metallic);

    // Initialize reflectance
    vec3 Lo = vec3(0.0);

    // TODO: Loop over each light in the scene, hard-coded as one for now
    // since we only have a single directional light.

    // Normalize sun direction to position vector
    vec3 L = normalize(-uSun.direction);
    vec3 H = normalize(V + L);
    //    float distance = length(uSun.direction - FragPos);
    //    float attenuation = 1.0 / (distance * distance);
    // vec3 radiance = uSun.color * attenuation; // No attenuation for directional lights
    vec3 radiance = uSun.color * uSun.intensity;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float cosTheta = clamp(dot(H, V), 0.0, 1.0);
    vec3 F = FresnelSchlick(cosTheta, F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    // Calculate energy conservation
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - uMaterial.metallic;

    // Add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * uMaterial.albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = uSun.color * 0.01 * uMaterial.albedo * uMaterial.ao;
    vec3 color = ambient + Lo;

    FragColor = vec4(color, 1.0);
}
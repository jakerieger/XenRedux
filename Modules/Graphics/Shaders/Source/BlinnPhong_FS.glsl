#version 460 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Sun {
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform Material uMaterial;
uniform Sun uSun;
uniform vec3 uViewPosition; // Camera position? edit: yes, apparently

in vec2 TexCoord;
in vec3 FragPos;
in vec3 FragNormal;

out vec4 FragColor;

void main() {
    vec3 normal = normalize(FragNormal);
    vec3 lightDir = normalize(-uSun.direction);
    vec3 viewDir = normalize(uViewPosition - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient = uMaterial.ambient * uSun.color * uSun.intensity;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = uMaterial.diffuse * diff * uSun.color * uSun.intensity;

    float spec = pow(max(dot(normal, halfwayDir), 0.0), uMaterial.shininess);
    vec3 specular = uMaterial.specular * spec * uSun.color * uSun.intensity;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
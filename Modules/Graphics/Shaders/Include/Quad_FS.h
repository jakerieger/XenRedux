const char* Quad_FS_Source = R"(
#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uRenderTexture;

void main() {
    FragColor = texture(uRenderTexture, TexCoords);
}
)";

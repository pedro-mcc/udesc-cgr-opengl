#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 hitPos;
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform sampler2D normalMap;

void main() {
    // Transforma a posição do ponto para o mundo 3D
    vs_out.hitPos = vec3(model * vec4(aPos, 1.0));

    // Lê a cor da textura do Normal Map e converte para direção
    vec3 sampledNormal = texture(normalMap, aTexCoords).rgb;
    vs_out.normal = normalize(sampledNormal * 2.0 - 1.0);

    gl_Position = vec4(aPos, 1.0); // O Geometry Shader assumirá o controle
}
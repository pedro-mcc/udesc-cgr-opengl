#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT {
    vec3 hitPos;
    vec3 normal;
} gs_in[];

uniform mat4 view;
uniform mat4 projection;

out vec3 lineColor;

void main() {
    vec3 hit = gs_in[0].hitPos;
    vec3 norm = gs_in[0].normal;

    // Desenha apenas a seta da Normal (Azul) para recriar a imagem do LearnOpenGL
    lineColor = vec3(0.0, 0.5, 1.0);
    gl_Position = projection * view * vec4(hit, 1.0);
    EmitVertex();
    
    // O valor '0.1' controla o comprimento da setinha azul na tela
    gl_Position = projection * view * vec4(hit + norm * 0.6, 1.0); 
    EmitVertex();
    
    EndPrimitive();
}
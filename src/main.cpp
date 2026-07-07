#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // Necessário para glm::value_ptr
#include <cmath>
#include <iostream>
#include <vector>

#include "camera.h"
#include "shader.h"

// --- IMPLEMENTAÇÃO DO TINY OBJ LOADER ---
#define TINYOBJLOADER_DISABLE_FAST_FLOAT
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// --- STB IMAGE IMPLEMENTATION ---
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void renderQuad();
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char *path);

// Configurações de tela
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Câmera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// Controle do Normal Mapping
bool normalMappingEnabled = true;
bool normalMapKeyPressed = false;

// --- FUNÇÃO PRINCIPAL ---
int main() {
    // 1. Inicializa GLFW e GLAD
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Cria a janela GLFW
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Normal Mapping", NULL, NULL);
    if(window == NULL){
        std::cerr << "ERROR::CREATE WINDOW GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERROR::LOAD GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // 2. Compilação dos Shaders:
    // shader: responsável pelo Normal Mapping (lê a normal map e aplica iluminação)
    // lampShader: shader simples para renderizar o objeto que representa a fonte de luz
    Shader shader("../shaders/normal_mapping.vs", "../shaders/normal_mapping.fr");
    Shader lampShader("../shaders/lamp.vs", "../shaders/lamp.fr");

    // 3. Carregamento das Texturas:
    unsigned int diffuseMap = loadTexture("../assets/brickwall_diffuse.jpg");
    unsigned int normalMap = loadTexture("../assets/brickwall_normal.jpg");

    shader.use();
    shader.setInt("diffuseMap", 0);
    shader.setInt("normalMap", 1);

    glm::vec3 lightPos(0.2f, 0.2f, 1.0f);

    // Render Loop
    while(!glfwWindowShouldClose(window))
    {
        // Cálculo do frame time para movimentação constante independente da taxa de FPS 
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 4. Lógica de Animação da Luz:
        // Movemos a posição da luz no tempo usando seno e cosseno para criar um órbita
        // Isso permite testar visualmente como a iluminação reage a diferentes ângulos
        float speed = 1.0f;
        float rangeX = 1.2f; 
        float rangeY = 0.8f;

        // Atualização dinâmica da posição da luz usando oscilação trigonométrica
        // Isso demonstra que o normal mapping reage a mudanças na direção da luz.
        lightPos.x = std::sin(glfwGetTime() * speed) * rangeX;
        lightPos.y = std::cos(glfwGetTime() * speed) * rangeY;
        lightPos.z = 0.5f;

        // configure view/projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);    
        glm::mat4 view = camera.GetViewMatrix();
        
        // 5. Renderização da Superfície (Parede):
        shader.use();
        shader.setBool("useNormalMap", normalMappingEnabled); // 'useNormalMap' controla se o shader aplica ou não o normal mapping
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // Define a transformação do modelo no espaço 3D
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
        shader.setMat4("model", model);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        
        // Ativa as texturas para o shader: a textura difusa e a normal map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        
        renderQuad();

        // 6. Renderização da Lâmpada (Visualizador da posição da luz):
        // Usa o shader simplificado para garantir que a luz apareça sempre branca (sem sombreamento)
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        lampShader.setMat4("model", model);
        renderQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        // 1. Definição básica: vértices do plano e suas coordenadas de textura (UVs)
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);  
        glm::vec2 uv4(1.0f, 1.0f);
        
        // Normal padrão (apontando para fora da tela)
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // 2. Cálculo das Tangentes e Bitangentes:
        // Para que o Normal Mapping funcione, precisamos mapear o espaço da textura (UV)
        // para o espaço geométrico do triângulo (XYZ). A tangente é a direção do eixo U da textura.

        // Triângulo 1
        glm::vec3 tangent1, bitangent1;        
        
        // Calculamos a aresta do triângulo (edge) e a variação das coordenadas UV (deltaUV)
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        // Fator 'f' para resolver o sistema linear:
        // edge1 = deltaUV1.x * T + deltaUV1.y * B
        // edge2 = deltaUV2.x * T + deltaUV2.y * B
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        // A tangente é calculada projetando as arestas nas direções das variações UV
        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        // Bitangente é o produto vetorial entre a normal e a tangente (ortogonal a ambos)
        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // Triângulo 2
        glm::vec3 tangent2, bitangent2;

        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        float quadVertices[] = {
            // positions            // normal         // texcoords  // tangent                          // bitangent
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        
        // 3. Configuração do VAO e VBO para o quad
        
        // Geração e vinculação do Vertex Array Object (VAO) e Vertex Buffer Object (VBO)
        glGenVertexArrays(1, &quadVAO);
        
        // Geração do VBO para armazenar os vértices do quad
        glGenBuffers(1, &quadVBO);
        
        // Vinculação do VAO para configurar os atributos de vértice
        glBindVertexArray(quadVAO);
        
        // Vinculação do VBO e envio dos dados de vértice para a GPU
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        
        // Envio dos dados de vértice para o buffer da GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        
        // Configuração dos atributos de vértice: posição, normal, coordenadas de textura, tangente e bitangente
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    // Renderiza o quad usando o VAO configurado
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// Processamento de entrada do teclado
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !normalMapKeyPressed) {
        normalMappingEnabled = !normalMappingEnabled; // Toggle normal mapping
        normalMapKeyPressed = true;
        
        // Debug for normal mapping ON and OFF
        std::cout << "Normal Mapping: " << (normalMappingEnabled ? "ON" : "OFF") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        normalMapKeyPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_W, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_S, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_A, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_D, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos; lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset); // Processa o movimento do mouse para controlar a câmera
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Falha ao carregar textura no caminho: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
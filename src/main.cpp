#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // Necessário para glm::value_ptr
#include <iostream>
#include <vector>

// --- IMPLEMENTAÇÃO DO TINY OBJ LOADER ---
#define TINYOBJLOADER_DISABLE_FAST_FLOAT
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// --- STB IMAGE IMPLEMENTATION ---
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// --- ESTRUTURAS ---
struct Vertex {
    float x, y, z;
    float r, g, b;
    float u, v;
};

// --- GLOBAIS DA CÂMERA ---
glm::vec3 cameraPos   = glm::vec3(0.0f, 1.0f,  5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

// Variáveis para controle do tempo
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Variáveis para controle do mouse
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 400.0f, lastY = 300.0f;
bool firstMouse = true;

// --- FUNÇÕES ---
// Função para carregar o modelo usando o TinyOBJLoader
std::vector<Vertex> loadModel(const char* path, std::string& outTexName) {
    tinyobj::attrib_t attrib; // Estrutura para armazenar os atributos do modelo (vértices, normais, texcoords, etc.)
    std::vector<tinyobj::shape_t> shapes; // Estrutura para armazenar as formas do modelo (cada forma pode conter um conjunto de triângulos)
    std::vector<tinyobj::material_t> materials; // Estrutura para armazenar os materiais do modelo (cores, texturas, etc.)
    std::string warn, err; // Variáveis para armazenar mensagens de aviso e erro do carregamento
    std::vector<Vertex> vertices; // Vetor para armazenar os vértices processados que serão usados para renderização

    // Extrai o diretório base do caminho do arquivo para resolver caminhos relativos de texturas
    std::string basePath = "";
    std::string pathStr(path);
    size_t pos = pathStr.find_last_of("/\\"); // Procura a última barra (Linux ou Windows)
    if (pos != std::string::npos) {
        basePath = pathStr.substr(0, pos + 1); // Corta a string até a barra (ex: "../assets/")
    }

    // Tenta carregar o modelo usando o TinyOBJLoader
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path, basePath.c_str())) { 
        std::cerr << "Erro: " << warn << err << std::endl; // Imprime mensagens de aviso e erro, se houver
        return vertices; // Retorna um vetor vazio em caso de falha no carregamento
    }

    // Verifica se há materiais e se o primeiro material tem um nome de textura difusa
    if (!materials.empty() && !materials[0].diffuse_texname.empty()) {
        outTexName = basePath + materials[0].diffuse_texname; // Armazena o nome da textura difusa para uso posterior
    }

    // Processa cada forma e seus índices para criar os vértices
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};
            vertex.x = attrib.vertices[3 * index.vertex_index + 0]; // Obtém a coordenada x do vértice
            vertex.y = attrib.vertices[3 * index.vertex_index + 1]; // Obtém a coordenada y do vértice
            vertex.z = attrib.vertices[3 * index.vertex_index + 2]; // Obtém a coordenada z do vértice

            vertex.r = 1.0f; vertex.g = 1.0f; vertex.b = 1.0f; // Define a cor do vértice como branco (pode ser modificado para usar cores do material)

            if (index.texcoord_index >= 0) {
                vertex.u = attrib.texcoords[2 * index.texcoord_index + 0]; // Obtém a coordenada u da textura
                vertex.v = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]; // Obtém a coordenada v da textura (inverte o valor para corrigir a orientação)
            }

            vertices.push_back(vertex); // Adiciona o vértice processado ao vetor de vértices
        }
    }
    return vertices;
}

// Callback para controle do mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos; lastY = ypos;

    float sensitivity = 0.1f;
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Processamento de entrada do teclado
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// --- FUNÇÃO PRINCIPAL ---
int main() {
    // Inicializa GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Trabalho CG - Visor de OBJ", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Falha ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Configura o contexto e o callback do mouse
    glfwMakeContextCurrent(window); // Configura o contexto para a janela criada
    glfwSetCursorPosCallback(window, mouse_callback); // Registra o callback para movimento do mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Esconde o cursor e captura o mouse para controle de câmera

    // Inicializa GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar o GLAD" << std::endl;
        return -1;
    }

    // --- SHADERS BÁSICOS (Movidos para cá, após inicializar o GLAD) ---
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n" // Atributo de coordenadas de textura
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n" // Variável de saída para o fragment shader
        "uniform mat4 model;\n" // Matriz de modelo
        "uniform mat4 view;\n" // Matriz de visão
        "uniform mat4 projection;\n" // Matriz de projeção
        "void main() {\n"
        "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n" // Transformação de vértice
        "   TexCoord = aTexCoord;\n" // Passa as coordenadas de textura para o fragment shader
        "}\n";

    // Fragment shader simples que apenas amostra a textura
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n" // Recebe as coordenadas de textura do vertex shader
        "in vec2 TexCoord;\n"
        "uniform sampler2D texture1;\n" // A textura que vamos amostrar
        "void main() {\n"
        "   FragColor = texture(texture1, TexCoord);\n" // Cor laranja
        "}\n";

    // Compila os shaders e cria o programa de shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); // Cria o shader de vértice
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Define o código-fonte do shader
    glCompileShader(vertexShader); // Compila o shader

    // Compila o shader de fragmento
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Cria o shader de fragmento
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // Define o código-fonte do shader de fragmento
    glCompileShader(fragmentShader); // Compila o shader de fragmento

    // Linka os shaders em um programa de shader
    unsigned int shaderProgram = glCreateProgram(); // Cria o programa de shader
    glAttachShader(shaderProgram, vertexShader); // Anexa o shader de vértice ao programa
    glAttachShader(shaderProgram, fragmentShader); // Anexa o shader de fragmento ao programa
    glLinkProgram(shaderProgram); // Linka o programa de shader

    glDeleteShader(vertexShader); // Os shaders já estão linkados, podemos deletar os objetos de shader
    glDeleteShader(fragmentShader); // Deleta o shader de fragmento

    // Cria a string que vai receber o nome da textura
    std::string textureFilename = "";

    // Carrega Modelo e Configura Buffers
    std::vector<Vertex> modelVertices = loadModel("/assets/12221_Cat_v1_l3.obj", textureFilename); // Carrega o modelo e obtém os vértices e o nome da textura    

    // Verifica se o vetor de vértices está vazio antes de prosseguir
    if (modelVertices.empty()) {
        std::cerr << "Encerrando: Vetor de vértices vazio." << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Configura os buffers de vértices e atributos
    unsigned int VAO, VBO; // Vertex Array Object e Vertex Buffer Object
    glGenVertexArrays(1, &VAO); // Gera um VAO
    glGenBuffers(1, &VBO); // Gera um VBO

    // Configura o VAO e VBO
    glBindVertexArray(VAO); // Vincula o VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Vincula o VBO
    glBufferData(GL_ARRAY_BUFFER, modelVertices.size() * sizeof(Vertex), modelVertices.data(), GL_STATIC_DRAW); // Envia os dados dos vértices para a GPU

    // Configura o Atributo de Posição (Location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Configura o layout do atributo de posição
    glEnableVertexAttribArray(0); // Habilita o atributo de posição

    // Configura o Atributo de Textura (Location = 1)
    // Os UVs estão depois do x,y,z e do nx,ny,nz (pulamos 6 floats)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Gera e Carrega a Textura
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); 

    // Configurações de repetição e filtro (padrão)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Carrega a imagem
    int width, height, nrChannels;
    if (!textureFilename.empty()) {
        std::cout << "Carregando textura informada pelo MTL: " << textureFilename << std::endl;
        
        // Passa o c_str() da string capturada para a biblioteca de imagem
        unsigned char *data = stbi_load(textureFilename.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "Falha ao carregar a imagem: " << textureFilename << std::endl;
        }
        stbi_image_free(data);
    } else {
        std::cout << "Aviso: O MTL deste modelo nao informou nenhum arquivo de textura." << std::endl;
    }

    // Habilita o teste de profundidade para renderizar corretamente os objetos 3D
    glEnable(GL_DEPTH_TEST);
    
    // Opcional: Modo wireframe para ver os triângulos claramente
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Loop Principal
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime(); // Obtém o tempo atual para calcular o deltaTime
        deltaTime = currentFrame - lastFrame; // Atualiza o tempo entre frames para controle de movimento suave
        lastFrame = currentFrame; // Atualiza o tempo do último frame

        processInput(window);

        // Limpa o buffer de cor e profundidade para preparar a nova renderização
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Define a cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa os buffers de cor e profundidade
        glUseProgram(shaderProgram); // Usa o programa de shader para renderizar o modelo

        // Configura as matrizes de projeção, visão e modelo
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f); // Matriz de projeção perspectiva
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); // Matriz de visão baseada na posição e direção da câmera
        glm::mat4 model = glm::mat4(1.0f); // Matriz de modelo inicializada como identidade
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f)); // Escala o modelo para caber melhor na cena

        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Roda o modelo para orientá-lo corretamente

        // ENVIA AS MATRIZES PARA O SHADER
        int modelLoc = glGetUniformLocation(shaderProgram, "model"); // Localização da matriz de modelo no shader
        int viewLoc  = glGetUniformLocation(shaderProgram, "view"); // Localização da matriz de visão no shader
        int projLoc  = glGetUniformLocation(shaderProgram, "projection"); // Localização da matriz de projeção no shader
        
        // Envia as matrizes para o shader usando glUniformMatrix4fv
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); // Envia a matriz de modelo
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)); // Envia a matriz de visão
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)); // Envia a matriz de projeção

        // Ativa a textura antes de desenhar
        glActiveTexture(GL_TEXTURE0); // Ativa a unidade de textura 0
        glBindTexture(GL_TEXTURE_2D, texture); // Vincula a textura que carregamos para ser usada no shader

        // Desenha o modelo usando glDrawArrays, que desenha os triângulos com base nos vértices fornecidos
        glBindVertexArray(VAO); // Vincula o VAO que contém as configurações dos vértices
        glDrawArrays(GL_TRIANGLES, 0, modelVertices.size()); // Desenha os triângulos usando os vértices do modelo

        glfwSwapBuffers(window); // Troca os buffers para exibir a nova renderização
        glfwPollEvents(); // Processa os eventos de entrada, como teclado e mouse
    }

    glfwTerminate();
    return 0;
}
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // Necessário para glm::value_ptr
#include <iostream>
#include <vector>

#include "Camera.h"
#include "Model.h"

// --- IMPLEMENTAÇÃO DO TINY OBJ LOADER ---
#define TINYOBJLOADER_DISABLE_FAST_FLOAT
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// --- STB IMAGE IMPLEMENTATION ---
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// --- VARIÁVEIS GLOBAIS ---

// Variáveis para controle do tempo
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Variáveis para controle do mouse
float lastX = 400.0f, lastY = 300.0f;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));


// Callback para controle do mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos; lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset); // Processa o movimento do mouse para controlar a câmera
}

// Processamento de entrada do teclado
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_W, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_S, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_A, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(GLFW_KEY_D, deltaTime);
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

    Model myModel("../assets/alemao.obj");

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
    std::string textureFilename = myModel.textureFilename;

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
        glm::mat4 view = camera.GetViewMatrix(); // Matriz de visão obtida da câmera
        
        glm::mat4 model = glm::mat4(1.0f); // Matriz de modelo inicializada como identidade
        model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); // Escala o modelo para caber melhor na cena
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Roda o modelo para orientá-lo corretamente

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

        myModel.Draw();

        glfwSwapBuffers(window); // Troca os buffers para exibir a nova renderização
        glfwPollEvents(); // Processa os eventos de entrada, como teclado e mouse
    }

    glfwTerminate();
    return 0;
}
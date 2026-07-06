#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <string>

#include "tiny_obj_loader.h"

struct Vertex {
    float x, y, z;
    float r, g, b;
    float u, v;
};

class Model {
    public:
        unsigned int VAO, VBO;
        unsigned int vertexCount;
        std::string textureFilename;

        Model(const char* path){
            std::vector<Vertex> vertices = loadModel(path, textureFilename);
            vertexCount = vertices.size();

            if(vertexCount == 0){
                std::cerr << "Erro: Modelo não carregou vértices ou arquivo vazio." << std::endl;
                return;
            }

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }

        void Draw() const {
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
    
    private:
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
};

#endif
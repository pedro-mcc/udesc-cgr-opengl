#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// ---------------------------------------------------------
// 1. Estrutura e Variáveis Globais
// ---------------------------------------------------------
typedef struct {
    float x, y, z;    
    float r, g, b, a; 
    float vx, vy, vz; 
    float life;       
} Particle;

#define MAX_PARTICLES 10000
Particle particles[MAX_PARTICLES]; // Array fixo em vez de std::vector

// ---------------------------------------------------------
// 2. Funções de Lógica (Em C usamos ponteiros em vez de referências)
// ---------------------------------------------------------
void respawnParticle(Particle* p) {
    p->x = ((rand() % 100) / 100.0f - 0.5f) * 0.4f;
    p->y = -1.0f;
    p->z = ((rand() % 100) / 100.0f - 0.5f) * 0.4f;

    p->vx = ((rand() % 100) / 100.0f - 0.5f) * 0.8f;
    p->vy = ((rand() % 100) / 100.0f) * 1.5f + 1.0f;
    p->vz = ((rand() % 100) / 100.0f - 0.5f) * 0.8f;

    p->r = 1.0f;
    p->g = 0.6f;
    p->b = 0.1f;
    p->a = 1.0f;

    p->life = ((rand() % 100) / 100.0f) * 0.8f + 0.2f;
}

void updateParticles(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &particles[i];
        if (p->life > 0.0f) {
            p->x += p->vx * dt;
            p->y += p->vy * dt;
            p->z += p->vz * dt;
            p->life -= dt * 0.7f;

            p->a = p->life;        
            p->g = p->life * 0.6f; 
        } else {
            respawnParticle(p);
        }
    }
}

// ---------------------------------------------------------
// 3. Callbacks
// ---------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (height == 0) height = 1; 
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// ---------------------------------------------------------
// 4. Main
// ---------------------------------------------------------
int main() {
    srand((unsigned int)time(NULL));

    if (!glfwInit()) {
        fprintf(stderr, "Erro ao iniciar GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Simulacao de Fogo em C", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Erro ao criar janela\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Setup inicial
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    framebuffer_size_callback(window, width, height);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].life = -1.0f; 
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    glEnable(GL_POINT_SMOOTH); 
    glPointSize(8.0f);         

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float dt = (float)(currentTime - lastTime);
        lastTime = currentTime;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        updateParticles(dt);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLoadIdentity();
        glTranslatef(0.0f, -0.5f, -5.0f); 

        // Em C, o array 'particles' já é um ponteiro para a memória
        glVertexPointer(3, GL_FLOAT, sizeof(Particle), &particles[0].x);
        glColorPointer(4, GL_FLOAT, sizeof(Particle), &particles[0].r);

        glDrawArrays(GL_POINTS, 0, MAX_PARTICLES);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// gcc fire.c -lglfw -lGL -lGLU -lm -o fire && ./fire
#define GLFW_INCLUDE_NONE
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>  
#include <GL/gl.h>       
#include <GL/glu.h>    
#include <stdbool.h>
#include <math.h>
#include <time.h>

void processInput(GLFWwindow *window);

// settings
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define NUM_PARTICLES 10000

typedef struct {
    float x, y, z; // position
    float r, g, b, a; // colors
    float velocity_x, velocity_y, velocity_z; // velocity
    float life; // life of particle
} Particle;

Particle particles[NUM_PARTICLES];

// Initialize fire particle
void respawnParticle(Particle* p) {
    // Position of particle
    p->x = ((rand() % 100) / 100.0f - 0.5f) * 0.4f; // -0.2 to +0.2
    p->y = -1.0f;
    p->z = ((rand() % 100) / 100.0f - 0.5f) * 0.4f; // -0.2 to +0.2

    // Velocity of particle
    p->velocity_x = ((rand() % 100) / 100.0f - 0.5f) * 0.8f; // -0.4 to +0.4
    p->velocity_y = ((rand() % 100) / 100.0f) * 1.5f + 1.0f; // +1.0 to +2.5
    p->velocity_z = ((rand() % 100) / 100.0f - 0.5f) * 0.8f; // -0.4 to +0.4

    // Particle colors
    p->r = 1.0f; // amount of red
    p->g = 0.6f; // amount of green
    p->b = 0.1f; // amount of blue
    p->a = 1.0f; // opacity

    // Particle life
    p->life = ((rand() % 100) / 100.0f) * 0.8f + 0.2f;
}

void updateParticles(float dt) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        Particle* p = &particles[i];
        if (p->life > 0.0f) {
            p->x += p->velocity_x * dt;
            p->y += p->velocity_y * dt;
            p->z += p->velocity_z * dt;
            p->life -= dt * 0.7f;

            p->a = p->life;
            p->g = p->life * 0.6f; 
        } else {
            respawnParticle(p);
        }
    }
}

/* function to reset our viewport after a window resize */
void resizeWindow(GLFWwindow* window, int width, int height )
{
    GLfloat ratio;
    if ( height == 0 ) height = 1;
    ratio = ( GLfloat )width / ( GLfloat )height;
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    gluPerspective( 45.0f, ratio, 0.1f, 100.0f );
    glMatrixMode( GL_MODELVIEW );
}

/* function to handle key press events */
void handleKeyPress(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwTerminate();
        exit(EXIT_SUCCESS);
    }
}

void drawGLScene(GLFWwindow* window) {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -3.0f); 

    glVertexPointer(3, GL_FLOAT, sizeof(Particle), &particles[0].x);
    glColorPointer(4, GL_FLOAT, sizeof(Particle), &particles[0].r);

    glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fireworks", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resizeWindow);
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    resizeWindow(window, width, height);
    

    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].life = -1.0f; 
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    resizeWindow(window, SCREEN_WIDTH, SCREEN_HEIGHT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    double lastTime = glfwGetTime();

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        float dt = (float)(currentTime - lastTime);
        lastTime = currentTime;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        updateParticles(dt);

        drawGLScene(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}


// gcc main.c robot.c -lglfw -lGL -lGLU -lm -o main && ./main
#define GLFW_INCLUDE_NONE
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>  // Header File For The GLFW Library
#include <GL/gl.h>       // Header File For The OpenGL32 Library
#include <GL/glu.h>      // Header File For The GLu32 Library
#include "robot.h"

void processInput(GLFWwindow* window);

// settings
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// Rotation
static GLfloat yRot = 0.0f;
static GLfloat xRot = 0.0f;

// Motion
static int shoulderR = 0, elbowR = 0;  // Right arm
static int shoulderL = 0, elbowL = 0;  // Left arm
static int hipR = 0, kneeR = 0;        // Right leg
static int hipL = 0, kneeL = 0;        // Left leg

static int animState = 0; // 0 = manual, 1 = wave, 2 = walk
static double animStartTime = 0.0;
static float currentWaveAngleR = 0.0f;

/* function to reset our viewport after a window resize */
void resizeWindow(GLFWwindow* window, int w, int h)
{
    GLfloat fAspect;

    // Prevent a divide by zero  
    if (h == 0)
        h = 1;

    // Set Viewport to window dimensions  
    glViewport(0, 0, w, h);

    fAspect = (GLfloat)w / (GLfloat)h;

    // Reset coordinate system  
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Produce the perspective projection  
    gluPerspective(35.0f, fAspect, 1.0, 40.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// This function does any needed initialization on the rendering context.  Here it sets up and initializes the lighting for the scene.  
void setupRC() {

    // Light values and coordinates  
    GLfloat  whiteLight[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat  sourceLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat  lightPos[] = { -10.f, 5.0f, 5.0f, 1.0f };

    glEnable(GL_DEPTH_TEST);    // Hidden surface removal  
    glFrontFace(GL_CCW);        // Counter clock-wise polygons face out  
    glEnable(GL_CULL_FACE);     // Do not calculate inside  

    // Enable lighting  
    glEnable(GL_LIGHTING);

    // Setup and enable light 0  
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);

    // Enable color tracking  
    glEnable(GL_COLOR_MATERIAL);

    // Set Material properties to follow glColor values  
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Black blue background  
    glClearColor(0.25f, 0.25f, 0.50f, 1.0f);

}

/* function to handle key press events */
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) yRot -= 5.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) yRot += 5.0f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) xRot -= 5.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) xRot += 5.0f;

	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) animState = 0; // Volta pro manual
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { 
        if(animState != 1) animStartTime = glfwGetTime(); 
        animState = 1; 
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { 
        if(animState != 2) animStartTime = glfwGetTime(); 
        animState = 2; 
    }

	if (animState == 0) {
        // Arms
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) shoulderR = (shoulderR + 5) % 360;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) elbowR = (elbowR + 5) % 360;
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) shoulderL = (shoulderL + 5) % 360;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) elbowL = (elbowL + 5) % 360;

        // Legs
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) hipR = (hipR + 5) % 360; 
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) kneeR = (kneeR + 5) % 360;
        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) hipL = (hipL + 5) % 360;
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) kneeL = (kneeL + 5) % 360;
    }

    yRot = (GLfloat)((const int)yRot % 360);
    xRot = (GLfloat)((const int)xRot % 360);
}

/* Here goes our drawing code */
void drawGLScene(GLFWwindow* window)
{
    GLUquadricObj* pObj;    // Quadric Object  

    // Clear the window with current clearing color  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Save the matrix state and do the rotations  
    glPushMatrix();

    // Move object back and do in place rotation  
    glTranslatef(0.0f, 0.0f, -1.0f);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    // Draw something  
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);

    // white
    glColor3f(1.0f, 1.0f, 1.0f);

    drawBody(pObj);

    // Head
    drawHead(pObj);

    // Arms
    drawArm(pObj, +1.0f, shoulderR, elbowR, currentWaveAngleR); // Right
    drawArm(pObj, -1.0f, shoulderL, elbowL, 0.0f);              // Left

    // Legs
    drawLeg(pObj, +1.0f, hipR, kneeR); // Right
    drawLeg(pObj, -1.0f, hipL, kneeL); // Left

    glPopMatrix();
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to open GLFW window\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Robot", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resizeWindow);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    resizeWindow(window, fbWidth, fbHeight);

    glEnable(GL_DEPTH_TEST);
    setupRC();

    GLUquadric* pObj = gluNewQuadric();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Processamento das Animações
        double currentTime = glfwGetTime();
        if (animState == 1) { // Wave
            ArmAnimation wave = waveAnimation(currentTime - animStartTime);
            shoulderR = wave.shoulder;
            elbowR = wave.elbow;
            currentWaveAngleR = wave.waveAngle;
        } 
        else if (animState == 2) { // Walk
            WalkAnimation walk = walkingAnimation(currentTime - animStartTime);
            shoulderR = walk.shoulderR;
            shoulderL = walk.shoulderL;
            hipR = walk.hipR;
            hipL = walk.hipL;
            kneeR = walk.kneeR;
            kneeL = walk.kneeL;
            currentWaveAngleR = 0.0f; 
        } 
        else { // Manual
            currentWaveAngleR = 0.0f;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        gluLookAt(0.0, 1.0, 5.0,  
            0.0, 1.0, 0.0,  
            0.0, 1.0, 0.0);

        drawGLScene(window); 

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gluDeleteQuadric(pObj);
    glfwTerminate();
    return 0;
}
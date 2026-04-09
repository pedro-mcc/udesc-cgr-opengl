// gcc robo.c -lglfw -lGL -lGLU -lm -o robot_glfw && ./robot_glfw

#define GLFW_INCLUDE_NONE
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>  // Header File For The GLFW Library
#include <GL/gl.h>       // Header File For The OpenGL32 Library
#include <GL/glu.h>      // Header File For The GLu32 Library

void processInput(GLFWwindow* window);

// settings
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// Rotation
static GLfloat yRot = 0.0f;
static GLfloat xRot = 0.0f;

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
void handleKeyPress(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwTerminate();
        exit(EXIT_SUCCESS);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yRot -= 5.0f;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yRot += 5.0f;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        xRot -= 5.0f;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        xRot += 5.0f;

    yRot = (GLfloat)((const int)yRot % 360);
    xRot = (GLfloat)((const int)xRot % 360);
}

void drawArm(GLUquadric* pObj, float side) {
    float x = 0.35f * side;

    // Shoulder
    glPushMatrix();
    glTranslatef(x, 1.2f, 0.0f);
    gluSphere(pObj, 0.1f, 26, 13);
    glPopMatrix();

    // Arm Sup
    glPushMatrix();
    glTranslatef(x, 1.15f, 0.0f);
    glRotatef(90.0f, 1, 0, 0);
    gluCylinder(pObj, 0.09f, 0.09f, 0.25f, 26, 13);
    glPopMatrix();

    // Elbow
    glPushMatrix();
    glTranslatef(x, 0.85f, 0.0f);
    gluSphere(pObj, 0.1f, 26, 13);
    glPopMatrix();

    // Arm Inf
    glPushMatrix();
    glTranslatef(x, 0.8f, 0.0f);
    glRotatef(90.0f, 1, 0, 0);
    gluCylinder(pObj, 0.09f, 0.09f, 0.25f, 26, 13);
    glPopMatrix();
}

// Desenha uma perna (lado = +1 para direito, -1 para esquerdo)
void drawLeg(GLUquadric* pObj, float side) {
    float x = 0.15f * side;

    // Hip
    glPushMatrix();
    glTranslatef(x, 0.55f, 0.0f);
    gluSphere(pObj, 0.1f, 26, 13);
    glPopMatrix();

    // Leg
    glPushMatrix();
    glTranslatef(x, 0.50f, 0.0f);
    glRotatef(90.0f, 1, 0, 0);
    gluCylinder(pObj, 0.09f, 0.09f, 0.35f, 26, 13);
    glPopMatrix();

    // Feet
    glPushMatrix();
    glTranslatef(x, 0.15f, 0.0f);
    gluSphere(pObj, 0.1f, 26, 13);
    glPopMatrix();
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
    glTranslatef(0.0f, -1.0f, -5.0f);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    // Draw something  
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);

    // white
    glColor3f(1.0f, 1.0f, 1.0f);

    // Body
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    gluCylinder(pObj, 0.25f, 0.25f, 0.7f, 26, 13);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.7f);
    gluDisk(pObj, 0.0f, 0.25f, 26, 1);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(pObj, 0.0f, 0.25f, 26, 1);
    glPopMatrix();

    glPopMatrix();

    // Arms
    drawArm(pObj, +1.0f); // Right
    drawArm(pObj, -1.0f); // Left

    // Legs
    drawLeg(pObj, +1.0f); // Right
    drawLeg(pObj, -1.0f); // Left

    // Head
    glPushMatrix();
    glTranslatef(0.0f, 1.3f, 0.0f);
    gluSphere(pObj, 0.25f, 26, 13);
    glPopMatrix();

    // Restore the matrix state  
    glPopMatrix();
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
        printf("Failed to open GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, resizeWindow);
    glfwSwapInterval(1);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    resizeWindow(window, SCREEN_WIDTH, SCREEN_HEIGHT);
    setupRC();

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        handleKeyPress(window);

        // render
        drawGLScene(window);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}


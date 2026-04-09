// gcc Application.c -lglfw -lGL -lGLU -lm -o castle_glfw && ./castle_glfw

#define GLFW_INCLUDE_NONE
#define _XOPEN_SOURCE 700 
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

/* function to handle wall model */
void castleWall(float x, float y, float z, float width, float height, float depth) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(width, height, depth);

    glBegin(GL_QUADS);
    glColor3f(0.7f, 0.7f, 0.7f);

    // Front
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 1.0f, 0.5f);
    glVertex3f(-0.5f, 1.0f, 0.5f);

    // Back
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 1.0f, -0.5f);
    glVertex3f(0.5f, 1.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);

    // Left
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(-0.5f, 1.0f, 0.5f);
    glVertex3f(-0.5f, 1.0f, -0.5f);

    // Right
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 1.0f, -0.5f);
    glVertex3f(0.5f, 1.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);

    // Top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, 1.0f, -0.5f);
    glVertex3f(-0.5f, 1.0f, 0.5f);
    glVertex3f(0.5f, 1.0f, 0.5f);
    glVertex3f(0.5f, 1.0f, -0.5f);

    // Botton
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);

    glEnd();
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
    glLoadIdentity();
    gluLookAt(
        15.0, 18.0, 25.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0
    );
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    // Draw something  
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);

    // Draw terrain
    glColor3f(0.3f, 0.6f, 0.3f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glEnd();

    // Castel Walls
    castleWall(0.0f, 0.0f, -5.0f, 10.0f, 4.0f, 0.5f);   // Front
    castleWall(0.0f, 0.0f, 5.0f, 10.0f, 4.0f, 0.5f);    // Back
    castleWall(-5.0f, 0.0f, 0.0f, 0.5f, 4.0f, 10.0f);   // Left
    castleWall(5.0f, 0.0f, 0.0f, 0.5f, 4.0f, 10.0f);    // Right

    // Tower Back - Right
    glColor3f(0.7f, 0.7f, 0.7f);
    glPushMatrix();
    glTranslatef(5.0f, 0.0f, 5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.0f, 2.0f, 6.0f, 26, 13);
    glPopMatrix();

    // Tower Front - Right
    glPushMatrix();
    glTranslatef(5.0f, 0.0f, -5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.0f, 2.0f, 6.0f, 26, 13);
    glPopMatrix();

    // Tower Back - Left
    glPushMatrix();
    glTranslatef(-5.0f, 0.0f, 5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.0f, 2.0f, 6.0f, 26, 13);
    glPopMatrix();

    // Tower Back - Right
    glPushMatrix();
    glTranslatef(-5.0f, 0.0f, -5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.0f, 2.0f, 6.0f, 26, 13);
    glPopMatrix();

    // Top Tower Front - Right
    glPushMatrix();
    glTranslatef(5.0f, 6.0f, 5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.5f, 2.5f, 2.0f, 26, 13);
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(pObj, 0.0f, 2.5f, 26, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.0f, 8.0f, 5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.5f, 0.0f, 2.0f, 26, 13);
    glPopMatrix();

    // Top Tower Front - Right
    glPushMatrix();
    glTranslatef(5.0f, 6.0f, -5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.5f, 2.5f, 2.0f, 26, 13);
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(pObj, 0.0f, 2.5f, 26, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.0f, 8.0f, -5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.5f, 0.0f, 2.0f, 26, 13);
    glPopMatrix();

    // Top Tower Front - Right
    glPushMatrix();
    glTranslatef(-5.0f, 6.0f, 5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.5f, 2.5f, 2.0f, 26, 13);
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(pObj, 0.0f, 2.5f, 26, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-5.0f, 8.0f, 5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.5f, 0.0f, 2.0f, 26, 13);
    glPopMatrix();

    // Top Tower Front - Right
    glPushMatrix();
    glTranslatef(-5.0f, 6.0f, -5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.5f, 2.5f, 2.0f, 26, 13);
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    gluDisk(pObj, 0.0f, 2.5f, 26, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-5.0f, 8.0f, -5.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 2.5f, 0.0f, 2.0f, 26, 13);
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


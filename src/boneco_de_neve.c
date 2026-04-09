// gcc boneco_de_neve.c -lglfw -lGL -lGLU -lm -o snowman_glfw && ./snowman_glfw

#define GLFW_INCLUDE_NONE
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>  // Header File For The GLFW Library
#include <GL/gl.h>       // Header File For The OpenGL32 Library
#include <GL/glu.h>      // Header File For The GLu32 Library

void processInput(GLFWwindow *window);

// settings
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// Rotation
static GLfloat yRot = 0.0f;

/* function to reset our viewport after a window resize */
void resizeWindow(GLFWwindow* window, int w, int h )
{
    GLfloat fAspect;  
  
    // Prevent a divide by zero  
    if(h == 0)  
        h = 1;  
  
    // Set Viewport to window dimensions  
    glViewport(0, 0, w, h);  
  
    fAspect = (GLfloat)w/(GLfloat)h;  
  
    // Reset coordinate system  
    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();  
  
    // Produce the perspective projection  
    gluPerspective(35.0f, fAspect, 1.0, 40.0);  
  
    glMatrixMode(GL_MODELVIEW);  
    glLoadIdentity();  
}

// This function does any needed initialization on the rendering context.  Here it sets up and initializes the lighting for the scene.  
void setupRC(){  

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
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,whiteLight);  
    glLightfv(GL_LIGHT0,GL_AMBIENT,sourceLight);  
    glLightfv(GL_LIGHT0,GL_DIFFUSE,sourceLight);  
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);  
    glEnable(GL_LIGHT0);  
  
    // Enable color tracking  
    glEnable(GL_COLOR_MATERIAL);  
      
    // Set Material properties to follow glColor values  
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);  
  
    // Black blue background  
    glClearColor(0.25f, 0.25f, 0.50f, 1.0f);  

}  

/* function to handle key press events */
void handleKeyPress(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwTerminate();
        exit(EXIT_SUCCESS);
    }

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) 
        yRot -= 5.0f;  
  
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)  
        yRot += 5.0f;  
                  
    yRot = (GLfloat)((const int)yRot % 360);  

}

/* Here goes our drawing code */
void drawGLScene(GLFWwindow* window)
{
    GLUquadricObj *pObj;    // Quadric Object  
      
    // Clear the window with current clearing color  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
  
    // Save the matrix state and do the rotations  
    glPushMatrix();

	// Move object back and do in place rotation  
	glTranslatef(0.0f, -1.0f, -5.0f);  
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);  

	// Draw something  
	pObj = gluNewQuadric();  
	gluQuadricNormals(pObj, GLU_SMOOTH);  

	// white
	glColor3f(1.0f, 1.0f, 1.0f);  

	// Main Body  
    gluSphere(pObj, 0.44f, 26, 13);

	// Mid section
    glPushMatrix();
        glTranslatef(0.0f, 0.55f, 0.0f);
        gluSphere(pObj, 0.34f, 26, 13);
    glPopMatrix();

	// Head
	glPushMatrix(); // save transform matrix state
		glTranslatef(0.0f, 1.0f, 0.0f);
		gluSphere(pObj, 0.24f, 26, 13);
	glPopMatrix(); // restore transform matrix state

	// Nose (orange)
	glColor3f(1.0f, 0.4f, 0.51f);  
	glPushMatrix();
		glTranslatef(0.0f, 1.0f, 0.2f);
		gluCylinder(pObj, 0.04f, 0.0f, 0.3f, 26, 13);  
	glPopMatrix();  

	// Eyes (black)
    glColor3f(0.0, 0.0f, 0.0f);
    glPushMatrix();
        glTranslatef(0.08f, 1.12f, 0.2f);
        gluSphere(pObj, 0.026f, 26, 13);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-0.08f, 1.12f, 0.2f);
        gluSphere(pObj, 0.026f, 26, 13);
    glPopMatrix();

	// Hat
    glPushMatrix();

        glTranslatef(0.0f, 1.15f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

        // Hat body
        gluCylinder(pObj, 0.24f, 0.24f, 0.5f, 26, 13);

        // Hat top
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.5f);
            gluDisk(pObj, 0.0f, 0.24f, 26, 1);
        glPopMatrix();

        // Hat brim
        gluCylinder(pObj, 0.35f, 0.35f, 0.05f, 26, 13);

        // Hat brim top
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, 0.05f);
            gluDisk(pObj, 0.24f, 0.35f, 26, 1);
        glPopMatrix();

        // Hat brim botton
        glPushMatrix();
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(pObj, 0.0f, 0.35f, 26, 1);
        glPopMatrix();
    glPopMatrix();

    // Buttons
    float R = 0.34f;                                // Mid section radius
    float centerY = 0.55f;                          // Mid section height center
    float thetas[3] = { 15.0f, 30.0f, 45.0f };      // Button angles

    for (int i = 0; i < 3; i++) {
        float theta = thetas[i] * M_PI / 180.0f;    // Degrees to radians
        float x = 0.0f;
        float z = R * cos(theta);
        float y = centerY + R * sin(theta);

        glPushMatrix();
        glTranslatef(x, y, z);
        gluSphere(pObj, 0.026f, 26, 13);
        glPopMatrix();
    }
    
    
     // Right Arms
    glColor3f(0.35f, 0.16f, 0.14f);
    glPushMatrix();
        glTranslatef(0.28f, 0.7f, 0.0f);
        glRotatef(90.0f, 0.3f, 1.0f, 0.0f);
        gluCylinder(pObj, 0.03f, 0.0f, 0.4f, 26, 13);
    glPopMatrix();

    // Left Arm
    glPushMatrix();
        glTranslatef(-0.28f, 0.7f, 0.0f);
        glRotatef(-90.0f, -0.3f, 1.0f, 0.0f);
        gluCylinder(pObj, 0.03f, 0.0f, 0.4f, 26, 13);
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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "robot.h"

void drawHead(GLUquadric* pObj) {
    glPushMatrix();
    glTranslatef(0.0f, 1.3f, 0.0f);
    gluSphere(pObj, 0.25f, 26, 13);
    glPopMatrix();
}

void drawBody(GLUquadric* pObj){
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
}

void drawArm(GLUquadric* pObj, float side, float shoulderAngle, float elbowAngle, float waveAngle) {
    float x = 0.35f * side;

    // Shoulder
    glPushMatrix();
    glTranslatef(x, 1.2f, 0.0f);
    glRotatef(-shoulderAngle, 1, 0, 0);
    gluSphere(pObj, 0.1f, 26, 13);

    // Arm sup
    glPushMatrix();
    glRotatef(90.0f, 1, 0, 0);
    gluCylinder(pObj, 0.09f, 0.09f, 0.25f, 26, 13);
    glPopMatrix();

    // Elbow
    glTranslatef(0.0f, -0.25f, 0.0f);
    glRotatef(-elbowAngle, 1, 0, 0);
    gluSphere(pObj, 0.1f, 26, 13);

    // Arm inf
    glPushMatrix();
    glRotatef(waveAngle, 0, 0, 1);  // rotaciona em torno do cotovelo
    glRotatef(90.0f, 1, 0, 0);      // orienta o cilindro para baixo
    gluCylinder(pObj, 0.08f, 0.08f, 0.25f, 26, 13);
    glPopMatrix();

    glPopMatrix();
}

// Desenha uma perna (lado = +1 para direito, -1 para esquerdo)
void drawLeg(GLUquadric* pObj, float side, float hipAngle, float kneeAngle) {
    float x = 0.15f * side;

    // Hip
    glPushMatrix();
    glTranslatef(x, 0.55f, 0.0f);
    glRotatef(hipAngle, 1, 0, 0);
    gluSphere(pObj, 0.1f, 26, 13);

    // Leg
    glPushMatrix();
    glRotatef(90.0f, 1, 0, 0);
    gluCylinder(pObj, 0.09f, 0.09f, 0.35f, 26, 13);
    glPopMatrix();

    // Foot
    glTranslatef(0.0f, -0.35f, 0.0f);
    glRotatef(0, 1, 0, 0);
    gluSphere(pObj, 0.1f, 26, 13);
    glPopMatrix();
}

ArmAnimation waveAnimation(double t){
    ArmAnimation animate = {0.0f, 0.0f, 0.0f};

    if (t < 1.0) {
        // Arm goes foward (0 to 45 degress)
        animate.shoulder = t * 45.0f;
    }
    else if (t < 2.0) {
        // Elbow from 0 to 100 degrees
        animate.shoulder = 45.0f;
        animate.elbow = (t - 1.0f) * 100.0f;
    }
    else if (t < 4.0) {
        // Wave 
        animate.shoulder = 45.0f;
        animate.elbow = 100.0f;
        animate.waveAngle = sin((t - 2.0f) * 6.0f) * 30.0f;
    }
    else if (t < 5.0) {
        // Back to initial position
        float progress = (t - 4.0f);
        animate.shoulder = 45.0f * (1.0f - progress);
        animate.elbow = 100.0f * (1.0f - progress);
        animate.waveAngle = 0.0f;
    }
    else {
        // End
        animate.shoulder = 0.0f;
        animate.elbow = 0.0f;
        animate.waveAngle = 0.0f;
    }

    return animate;
}

WalkAnimation walkingAnimation(double t) {
    WalkAnimation anim;
    float speed = 6.0f;
    float swing = 35.0f;

    // Braços balançam na direção oposta às pernas
    anim.shoulderR = sin(t * speed) * swing;
    anim.shoulderL = -sin(t * speed) * swing;

    // Movimento do quadril
    anim.hipR = -sin(t * speed) * swing;
    anim.hipL = sin(t * speed) * swing;

    // O joelho só deve dobrar na perna que está sendo movida para a frente e para o alto.
    anim.kneeR = (sin(t * speed) < 0) ? -sin(t * speed) * 40.0f : 0.0f;
    anim.kneeL = (sin(t * speed) > 0) ?  sin(t * speed) * 40.0f : 0.0f;

    return anim;
}
typedef struct arm{
    float shoulder;
    float elbow;
    float waveAngle;
} ArmAnimation;

typedef struct walk {
    float shoulderR;
    float shoulderL;
    float hipR;
    float hipL;
    float kneeR;
    float kneeL;
} WalkAnimation;

void drawHead(GLUquadric* pObj);
void drawBody(GLUquadric* pObj);
void drawArm(GLUquadric* pObj, float side, float shoulderAngle, float elbowAngle, float waveAngle);
void drawLeg(GLUquadric* pObj, float side, float hipAngle, float kneeAngle);
ArmAnimation waveAnimation(double t);
WalkAnimation walkingAnimation(double t);
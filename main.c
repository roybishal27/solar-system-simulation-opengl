#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.1416
#define STARS_COUNT 500
#define ASTEROID_COUNT 200

float camX = 0.0, camY = 40.0, camZ = 70.0;
float spin = 0.0;

struct Point3D
{
    float x, y, z;
};

struct Point3D stars[STARS_COUNT];
struct Point3D asteroids[ASTEROID_COUNT];

struct Planet
{
    char name[20];
    float distance;
    float radius;
    float orbitSpeed;
    float spinSpeed;
    float angle;
    float r, g, b;
};

struct Planet planets[8] = {
    {"Mercury", 7.0, 0.4, 4.0, 5.0, 0.0, 0.7, 0.7, 0.7},
    {"Venus", 10.0, 0.6, 3.0, 4.0, 0.0, 0.9, 0.6, 0.2},
    {"Earth", 13.0, 0.65, 2.5, 6.0, 0.0, 0.0, 0.5, 1.0},
    {"Mars", 17.0, 0.5, 2.0, 5.5, 0.0, 1.0, 0.2, 0.0},
    {"Jupiter", 26.0, 1.8, 1.2, 10.0, 0.0, 0.8, 0.6, 0.4},
    {"Saturn", 35.0, 1.5, 0.9, 9.0, 0.0, 0.9, 0.8, 0.5},
    {"Uranus", 43.0, 1.1, 0.6, 7.0, 0.0, 0.4, 0.9, 0.9},
    {"Neptune", 52.0, 1.0, 0.4, 6.5, 0.0, 0.1, 0.1, 0.8}};

void printText(float x, float y, float z, char *text)
{
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos3f(x, y, z);
    for (int i = 0; text[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[i]);
    }
    glEnable(GL_LIGHTING);
}

void drawOrbit(float radius)
{
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.3, 0.3, 0.3);
    for (int i = 0; i < 360; i++)
    {
        float rad = i * PI / 180.0;
        glVertex3f(radius * cos(rad), 0.0, radius * sin(rad));
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void initScene()
{
    glClearColor(0.0, 0.0, 0.0, 1.0); // black background
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    // lighting setup
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Disable default global ambient light to make shadows dark
    GLfloat globalAmbient[] = {0.02, 0.02, 0.02, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    GLfloat ambientLight[] = {0.0, 0.0, 0.0, 1.0}; // totally dark for night side
    GLfloat diffuseLight[] = {1.2, 1.2, 1.2, 1.0}; // bright light for day side

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

    // making random stars
    for (int i = 0; i < STARS_COUNT; i++)
    {
        stars[i].x = (rand() % 400) - 200;
        stars[i].y = (rand() % 400) - 200;
        stars[i].z = (rand() % 400) - 200;
    }

    // Asteroids between mars and saturn
    for (int i = 0; i < ASTEROID_COUNT; i++)
    {
        float d = 20.0 + (rand() % 40) / 10.0;
        float a = (rand() % 360) * PI / 180.0;
        asteroids[i].x = d * cos(a);
        asteroids[i].y = (rand() % 10 - 5) / 10.0; // slightly up and down
        asteroids[i].z = d * sin(a);
    }
}

void drawingScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Camera Setup
    gluLookAt(camX, camY, camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    GLfloat lightPos[] = {0.0, 0.0, 0.0, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Drawing Stars
    glDisable(GL_LIGHTING);
    glColor3f(0.8, 0.8, 0.8);
    glBegin(GL_POINTS);
    for (int i = 0; i < STARS_COUNT; i++)
    {
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    }
    glEnd();

    // Drawing Asteroids
    glColor3f(0.5, 0.4, 0.3);
    glBegin(GL_POINTS);
    for (int i = 0; i < ASTEROID_COUNT; i++)
    {
        glVertex3f(asteroids[i].x, asteroids[i].y, asteroids[i].z);
    }
    glEnd();
    glEnable(GL_LIGHTING);

    // Drawing Sun
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 0.6, 0.0); // orange-yellow
    glutSolidSphere(3.5, 40, 40);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // Drawing planets
    for (int i = 0; i < 8; i++)
    {
        drawOrbit(planets[i].distance); // Orbit

        glPushMatrix();
        // Circulating around Sun
        glRotatef(planets[i].angle, 0.0, 1.0, 0.0);
        glTranslatef(planets[i].distance, 0.0, 0.0);

        // Name of the planets
        printText(0.0, planets[i].radius + 0.8, 0.0, planets[i].name);

        // Circulating on own axis
        glPushMatrix();
        glRotatef(spin * planets[i].spinSpeed, 0.0, 1.0, 0.0);

        glColor3f(planets[i].r, planets[i].g, planets[i].b);
        glutSolidSphere(planets[i].radius, 30, 30);

        // Drawing ring for saturn
        if (i == 5)
        {
            glDisable(GL_LIGHTING);

            glRotatef(90.0, 1.0, 0.0, 0.0);

            glColor3f(0.8, 0.7, 0.6);
            glutSolidTorus(0.04, planets[i].radius + 0.6, 20, 50);

            glColor3f(0.6, 0.5, 0.4);
            glutSolidTorus(0.05, planets[i].radius + 1.1, 20, 50);

            glColor3f(0.7, 0.6, 0.5);
            glutSolidTorus(0.04, planets[i].radius + 1.6, 20, 50);

            glEnable(GL_LIGHTING);
        }
        glPopMatrix();

        // drawing moon for earth
        if (i == 2)
        {
            glRotatef(planets[i].angle * 4, 0.0, 1.0, 0.0); // speed of moon
            glTranslatef(1.5, 0.0, 0.0);
            printText(0.0, 0.5, 0.0, "Moon");
            glColor3f(0.9, 0.9, 0.9);
            glutSolidSphere(0.35, 15, 15);
        }
        glPopMatrix();
    }

    glutSwapBuffers();
}

void updateLogic(int val)
{
    spin += 1.0f; // speed of rotating own axis
    for (int i = 0; i < 8; i++)
    {
        planets[i].angle += planets[i].orbitSpeed * 0.15;
        if (planets[i].angle > 360.0)
        {
            planets[i].angle -= 360.0;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, updateLogic, 0);
}

void resizeWindow(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / h, 1.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyInput(unsigned char key, int x, int y)
{
    if (key == 27)
        exit(0);
    if (key == 'w' || key == 'W')
        camZ -= 2.0;
    if (key == 's' || key == 'S')
        camZ += 2.0;
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("My Solar System Project");

    initScene();

    glutDisplayFunc(drawingScene);
    glutReshapeFunc(resizeWindow);
    glutTimerFunc(0, updateLogic, 0);
    glutKeyboardFunc(keyInput);

    // Terminal user guide
    printf("Project Started.\n");
    printf("Press 'W' to Zoom In\n");
    printf("Press 'S' to Zoom Out\n");
    printf("Press 'ESC' to Exit\n");

    glutMainLoop();
    return 0;
}
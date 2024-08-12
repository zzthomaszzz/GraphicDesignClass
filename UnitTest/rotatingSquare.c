#include <stdio.h>
#include <freeglut.h>
#include <math.h>

#define PI 3.14159265

#define DEG_TO_RAD PI/180.0


void drawCircle(GLfloat radius)
{

	GLfloat vertexList[60][2];

	int increment = 360 / 60;
	for (int i = 0; i < 60; i++) {
		vertexList[i][0] = cos(DEG_TO_RAD * (i * increment)) * radius;
		vertexList[i][1] = sin(DEG_TO_RAD * (i * increment)) * radius;
	}
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.0, 0.0);
	for (int i = 0; i < 60; i++) {
		glVertex2f(vertexList[i][0], vertexList[i][1]);
	}
	glVertex2f(vertexList[0][0], vertexList[0][1]);
	glEnd();
}

void initializeGL(void)
{
	// set background color to be black
	glClearColor(0, 0, 0, 1);

	// set the drawing to be white
	glColor3f(1.0, 1.0, 1.0);

	// set the point size
	glPointSize(15.0);

	// set window mode to 2D orthographic and set the window size 
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

void display() 
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawCircle(0.5f);
	glutSwapBuffers();
}


void main(int argc, char** argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Rotating Square");

	glutDisplayFunc(display);

	initializeGL();

	glutMainLoop();
}

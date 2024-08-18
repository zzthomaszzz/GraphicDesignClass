/******************************************************************************
 *
 * Animation v1.0 (23/02/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene.
 *
 ******************************************************************************/

#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

#define KEY_EXIT			27 // Escape key.

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/


void main(int argc, char** argv);
void init(void);
void think(void);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

#define PI 3.14159265

#define DEG_TO_RAD PI/180.0

#define MAX_PARTICLES 1000

int snowOn = 0;
int snowFrame = 0;
int windOnLeft = 0;
int windOnRight = 0;
GLfloat maxHorizontalVelocity = 5.0f;

//This is the x axis and y axis size
GLfloat dimension[4] = {-50.0, 50.0, -50.0, 50.0};

//Color
float snowManBody[6] = { 1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.8f };
float snowManEye[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float snowManNose[6] = { 1.0f, 0.5f, 0.0, 1.0f, 0.5f, 0.0f };
float ground[6] = { 0.9f, 0.8f, 1.0f, 0.5f, 0.7f, 0.6f };
float snow[6] = { 1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.5f };
float sky[6] = { 0.8f, 0.8f, 0.9f };

GLfloat skyCoord[4][2] = { {0.0, 0.0},
								{0.0, 0.0},
	                            {0.0, 0.0},
	                            {0.0, 0.0} };
GLfloat terrainCoord[4][2] = { {0.0, -10.0},
								{0.0, -10.0},
								{0.0, 0.0},
								{0.0, 0.0} };

typedef struct {
	float x;
	float y;
} Position2;

typedef struct {
	Position2 position;
	float size;
	float dy;
	float dx;
	int active;
} Particle_t;

Particle_t particleSystem[MAX_PARTICLES];

int particleCount = 0;

void generateTerrain() {
	terrainCoord[0][0] = (rand() % 100 + 1) * 0.1 + dimension[0];
	terrainCoord[1][0] = dimension[1] - (rand() % 100 + 1) * 0.1;
	terrainCoord[2][0] = dimension[1] + 5.0f;
	terrainCoord[2][1] = dimension[2];
	terrainCoord[3][0] = dimension[0] - 5.0f;
	terrainCoord[3][1] = dimension[2];


}

void generateSky() {
	skyCoord[0][0] = dimension[0];
	skyCoord[0][1] = dimension[3];
	skyCoord[1][0] = dimension[1];
	skyCoord[1][1] = dimension[3];
	skyCoord[2][0] = dimension[1];
	skyCoord[2][1] = dimension[2];
	skyCoord[3][0] = dimension[0];
	skyCoord[3][1] = dimension[2];
}

void drawSky() {
	glBegin(GL_POLYGON);
	glColor3f(sky[0], sky[1], sky[2]);
	glVertex2f(skyCoord[0][0], skyCoord[0][1]);
	glVertex2f(skyCoord[1][0], skyCoord[1][1]);
	glVertex2f(skyCoord[2][0], skyCoord[2][1]);
	glVertex2f(skyCoord[3][0], skyCoord[3][1]);


	glEnd();
}

void drawTerrain() {

	glBegin(GL_POLYGON);
	glColor3f(ground[0], ground[1], ground[2]);
	glVertex2f(terrainCoord[3][0], terrainCoord[3][1]);
	glVertex2f(terrainCoord[2][0], terrainCoord[2][1]);

	glColor3f(ground[3], ground[4], ground[5]);
	glVertex2f(terrainCoord[1][0], terrainCoord[1][1]);
	glVertex2f(terrainCoord[0][0], terrainCoord[0][1]);

	glEnd();

}

void drawCircle(GLfloat radius, GLfloat x, GLfloat y, float Color[])
{

	GLfloat increment = 10.0f;
	glBegin(GL_TRIANGLE_FAN);

	glColor3f(Color[0], Color[1], Color[2]);
	glVertex2f(0.0 + x, 0.0 + y);

	glColor3f(Color[3], Color[4], Color[5]);
	for (float i = 0; i * increment <= 360; i++) {
		glVertex2f((cos(DEG_TO_RAD * (i * increment)) * radius) + x, (sin(DEG_TO_RAD * (i * increment)) * radius) + y);
	}
	glVertex2f((cos(0) * radius) + x, (sin(0) * radius) + y);

	glEnd();
}

void drawSnow() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particleSystem[i].active != 0) {
			drawCircle(particleSystem[i].size, particleSystem[i].position.x, particleSystem[i].position.y,snow);
		}
	}
}

void spawnSnow() {
	Position2 newPos = { ((rand() % 100) - 49), dimension[3] + 10};
	Particle_t newSnow = { newPos, ((rand() % 10) + 1.0) / 8.0f, (rand() % 10 + 1.0f) / 2.0f, 0.0f, 1};
	particleSystem[particleCount] = newSnow;
	particleCount++;
	if (particleCount >= MAX_PARTICLES) {
		particleCount = 0;
	}
}

void updateSnow() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particleSystem[i].active != 0) {
			if (windOnLeft == 1) {
				particleSystem[i].dx -= 0.05f;
				if (particleSystem[i].dx <= -maxHorizontalVelocity) {
					particleSystem[i].dx = -maxHorizontalVelocity;
				}
			}
			else if (windOnRight) {
				particleSystem[i].dx += 0.05f;
				if (particleSystem[i].dx >= maxHorizontalVelocity) {
					particleSystem[i].dx = maxHorizontalVelocity;
				}
			}
			else {
				if (particleSystem[i].dx > 0) {
					particleSystem[i].dx -= 0.05f;
				}
				else if (particleSystem[i].dx < 0) {
					particleSystem[i].dx += 0.05f;
				}
			}
			particleSystem[i].position.y -= particleSystem[i].dy * FRAME_TIME_SEC;
			particleSystem[i].position.x += particleSystem[i].dx * FRAME_TIME_SEC;

			particleSystem[i].size -= 0.0005f;
			if (particleSystem[i].size < 0.01f) {
				particleSystem[i].active = 0;
			}
		}
		if (particleSystem[i].position.y < dimension[2]) {
			particleSystem[i].active = 0;
		}
	}
}

void showInfo() {

	glRasterPos3f(-48.0f, 35.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, "\'esc\' to exist");

	glRasterPos3f(-48.0f, 30.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, "\'q\' for snow particle");

	glRasterPos3f(-48.0f, 25.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, "\'a\' for wind left");

	glRasterPos3f(-48.0f, 20.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, "\'d\' for wind right");

	glRasterPos3f(-48.0f, 15.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, "\'s\' to stop wind");

	glRasterPos3f(-48.0f, 10.0f, 0.0f);
	if (snowOn) {
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, "Snow On");
	}
	else {
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, "Snow Off");
	}

	glRasterPos3f(-48.0f, 5.0f, 0.0f);
	if (windOnRight == 1 || windOnLeft == 1) {
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, "Wind On");
	}
	else {
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, "Wind Off");
	}
}

 /******************************************************************************
  * Entry Point (don't put anything except the main function here)
  ******************************************************************************/

void main(int argc, char** argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Animation");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT);

	drawSky();

	drawSnow();
	
	drawTerrain();

	drawCircle(5.0f, -15.0f, -10.0f, snowManBody);
	drawCircle(4.8f, -15.0f, -2.0f, snowManBody);
	drawCircle(0.6f, -17.0f, -2.5f, snowManEye);
	drawCircle(0.6f, -13.0f, -2.5f, snowManEye);
	drawCircle(0.7f, -15.0, -2.8f, snowManNose);

	showInfo();

	glutSwapBuffers();
}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{

}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
		/*
			TEMPLATE: Add any new character key controls here.

			Rather than using literals (e.g. "d" for diagnostics), create a new KEY_
			definition in the "Keyboard Input Handling Setup" section of this file.
		*/

	case 'q':
		if (snowOn == 0) {
			snowOn = 1;
		}
		else if (snowOn == 1) {
			snowOn = 0;
		}
		break;

	case 'a':
		windOnLeft = 1;
		windOnRight = 0;
		break;
	case 'd':
		windOnRight = 1;
		windOnLeft = 0;
		break;
	case 's':
		windOnLeft = 0;
		windOnRight = 0;
		break;
	case KEY_EXIT:
		exit(0);
		break;
	}
}

void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.

	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

void init(void)
{
	glClearColor(0, 0, 0, 1);

	glColor3f(1.0, 1.0, 1.0);

	glPointSize(10.0);

	gluOrtho2D(dimension[0], dimension[1], dimension[2], dimension[3]);

	//Init terrain and sky
	generateTerrain();
	generateSky();

}

void think(void)
{
	
	if (snowOn) {
		if (snowFrame >= 5){
			spawnSnow();
			snowFrame = 0;
		}
		snowFrame ++;
	}

	updateSnow();
}

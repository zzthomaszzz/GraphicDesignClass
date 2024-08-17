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

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Define all character keys used for input (add any new key definitions here).
 // Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
 // characters typed by the user to lowercase, so the SHIFT key is ignored.

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

GLfloat dimension[4] = {-50.0, 50.0, -50.0, 50.0};

float snowManBody[6] = { 1.0f, 1.0f, 1.0f, 0.7f, 0.7f, 0.8f };
float snowManEye[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float snowManNose[6] = { 1.0f, 0.5f, 0.0, 1.0f, 0.5f, 0.0f };
float ground[6] = { 0.9f, 0.8f, 1.0f, 0.5f, 0.7f, 0.6f };
float snow[6] = { 1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.5f };
float sky[6] = { 0.8f, 0.7f, 0.9f, 0.4f, 0.7f, 0.6f };

GLfloat topLeftTerrain[2] = { 0.0, -10.0 };
GLfloat topRightTerrain[2] = { 0.0, -10.0 };
GLfloat botLeftTerrain[2] = { 0.0, 0.0};
GLfloat botRightTerrain[2] = { 0.0, 0.0};
GLfloat skyCoord[4][2] = { {0.0, 0.0},
								{0.0, 0.0},
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
	int active;
} Particle_t;

Particle_t particleSystem[MAX_PARTICLES];

int particleCount = 0;

void generateTerrain() {
	topLeftTerrain[0] = (rand() % 100 + 1) * 0.1 + dimension[0];
	topRightTerrain[0] = dimension[1] - (rand() % 100 + 1) * 0.1;
	botLeftTerrain[0] = dimension[0] - 5.0f;
	botLeftTerrain[1] = dimension[2];
	botRightTerrain[0] = dimension[1] + 5.0f;
	botRightTerrain[1] = dimension[2];

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

void drawSky(GLfloat skyCoor[4][2], float Color[]) {
	glBegin(GL_POLYGON);
	glColor3f(Color[0], Color[1], Color[2]);
	glVertex2f(skyCoor[0][0], skyCoor[0][1]);
	glVertex2f(skyCoor[1][0], skyCoor[1][1]);
	glVertex2f(skyCoor[2][0], skyCoor[2][1]);
	glVertex2f(skyCoor[3][0], skyCoor[3][1]);


	glEnd();
}

void drawTerrain(GLfloat topLeft[], GLfloat topRight[], GLfloat botLeft[], GLfloat botRight[], float Color[]) {

	glBegin(GL_POLYGON);
	glColor3f(Color[0], Color[1], Color[2]);
	glVertex2f(botLeft[0], botLeft[1]);
	glVertex2f(botRight[0], botRight[1]);

	glColor3f(Color[3], Color[4], Color[5]);
	glVertex2f(topRight[0], topRight[1]);
	glVertex2f(topLeft[0], topLeft[1]);

	glEnd();

}

void drawCircle(GLfloat radius, GLfloat x, GLfloat y, float Color[])
{

	GLfloat increment = 10.0f;
	glBegin(GL_TRIANGLE_FAN);

	//Origin of the circle
	glColor3f(Color[0], Color[1], Color[2]);
	glVertex2f(0.0 + x, 0.0 + y);

	//Other vertex surrounding the circle
	glColor3f(Color[3], Color[4], Color[5]);
	for (float i = 0; i * increment <= 360; i++) {
		glVertex2f((cos(DEG_TO_RAD * (i * increment)) * radius) + x, (sin(DEG_TO_RAD * (i * increment)) * radius) + y);
	}
	// repeat the first vertex to finish connecting the circle, without this the circle would look like a pizza missing a slice.
	glVertex2f((cos(0) * radius) + x, (sin(0) * radius) + y);

	glEnd();
}

void drawSnow() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particleSystem[i].active != 0) {
//Remember to change snowman BODY to snow color
			drawCircle(particleSystem[i].size, particleSystem[i].position.x, particleSystem[i].position.y,snow);
		}
	}
}

void spawnSnow() {
	Position2 newPos = { ((rand() % 100) - 49), dimension[3] + 10};
	Particle_t newSnow = { newPos, ((rand() % 10) + 1.0) / 8.0f, (rand() % 10 + 1.0f) / 2.0f, 1};
	particleSystem[particleCount] = newSnow;
	particleCount++;
}

void updateSnow() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particleSystem[i].active != 0) {
			particleSystem[i].position.y -= particleSystem[i].dy * FRAME_TIME_SEC;
		}
		if (particleSystem[i].position.y < dimension[2]) {
			particleSystem[i].active = 0;
		}
	}
}

void showInfo() {

	char particleText[20] = "Particle ";
	char number[4];

	glRasterPos3f(-48.0f, 30.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, "\'q\' for snow particle");
	glRasterPos3f(-48.0f, 25.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, particleText);
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

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
void display(void)
{
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR DRAWING CODE

		Separate reusable pieces of drawing code into functions, which you can add
		to the "Animation-Specific Functions" section below.

		Remember to add prototypes for any new functions to the "Animation-Specific
		Function Prototypes" section near the top of this template.
	*/

	glClear(GL_COLOR_BUFFER_BIT);

	drawSky(skyCoord, sky);

	drawSnow();
	
	drawTerrain(topLeftTerrain, topRightTerrain, botLeftTerrain, botRightTerrain, ground);

	drawCircle(5.0f, 0.0f, 0.0f, snowManBody);
	drawCircle(4.8f, 0.0f, 8.0f, snowManBody);
	drawCircle(0.6f, -2.0f, 7.5f, snowManEye);
	drawCircle(0.6f, 2.0f, 7.5f, snowManEye);
	drawCircle(0.7f, 0.0, 7.2f, snowManNose);

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

	case 113:
		if (snowOn == 0) {
			snowOn = 1;
		}
		else if (snowOn == 1) {
			snowOn = 0;
		}
		break;
	case KEY_EXIT:
		exit(0);
		break;
	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
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




 /*
	 Initialise OpenGL and set up our scene before we begin the render loop.
 */
void init(void)
{
	// set background color to be black
	glClearColor(0, 0, 0, 1);

	// set the drawing to be white
	glColor3f(1.0, 1.0, 1.0);

	// set the point size
	glPointSize(10.0);

	// set window mode to 2D orthographic and set the window size 
	gluOrtho2D(dimension[0], dimension[1], dimension[2], dimension[3]);

	generateTerrain();
	generateSky();

}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
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
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR ANIMATION/SIMULATION CODE

		In this function, we update all the variables that control the animated
		parts of our simulated world. For example: if you have a moving box, this is
		where you update its coordinates to make it move. If you have something that
		spins around, here's where you update its angle.

		NOTHING CAN BE DRAWN IN HERE: you can only update the variables that control
		how everything will be drawn later in display().

		How much do we move or rotate things? Because we use a fixed frame rate, we
		assume there's always FRAME_TIME milliseconds between drawing each frame. So,
		every time think() is called, we need to work out how far things should have
		moved, rotated, or otherwise changed in that period of time.

		Movement example:
		* Let's assume a distance of 1.0 GL units is 1 metre.
		* Let's assume we want something to move 2 metres per second on the x axis
		* Each frame, we'd need to update its position like this:
			x += 2 * (FRAME_TIME / 1000.0f)
		* Note that we have to convert FRAME_TIME to seconds. We can skip this by
		  using a constant defined earlier in this template:
			x += 2 * FRAME_TIME_SEC;

		Rotation example:
		* Let's assume we want something to do one complete 360-degree rotation every
		  second (i.e. 60 Revolutions Per Minute, or RPM).
		* Each frame, we'd need to update our object's angle like this (we'll use the
		  FRAME_TIME_SEC constant as per the example above):
			a += 360 * FRAME_TIME_SEC;

		This works for any type of "per second" change: just multiply the amount you'd
		want to move in a full second by FRAME_TIME_SEC, and add or subtract that
		from whatever variable you're updating.

		You can use this same approach to animate other things like color, opacity,
		brightness of lights, etc.
	*/
}

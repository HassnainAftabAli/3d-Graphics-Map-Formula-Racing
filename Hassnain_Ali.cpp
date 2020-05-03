#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "camera.h"
#include "texture.h"
#include "LightSource.h"
#include "Material.h"
#include "Model.h"
#include "stb_image.h"
#include <sstream>
#include <map>
#include <vector>
#include <time.h>

#define WINDOW_WIDTH  1600
#define WINDOW_HEIGHT 900

#define HALF_WIDTH (WINDOW_WIDTH >> 1)     
#define HALF_HEIGHT (WINDOW_HEIGHT >> 1)

// Perspective Camera Parameters
#define FOVY 70.0f
#define NEAR_PLANE 0.01f
#define FAR_PLANE  500.0f

#define  TIMER_PERIOD  16 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define MOUSE_SENSIVITY 4.0f

#define D2R 0.0174532

#define MAP_SIZE 15

/* Global Variables for Template File */
bool upKey = false, downKey = false, rightKey = false, leftKey = false;
bool wKey = false, sKey = false, aKey = false, dKey = false, spaceKey = false;
int  winWidth, winHeight; // current Window width and height
bool mode = true;  // F1 key to toggle drawing mode between GL_LINE and GL_FILL
bool capture = true;  // Middle Mouse button set/unset mouse capture
int mouse_x = HALF_WIDTH;
int mouse_y = HALF_HEIGHT;
int trackMap[MAP_SIZE][MAP_SIZE] = { 0 };
Model drawTile;
Model raceCar;
Model trees[];
#define TREES_NUM 35
int tree_type[TREES_NUM];
float tree_xAxis[TREES_NUM];
float tree_yAxis[TREES_NUM];
float tree_zAxis[TREES_NUM];

#define FLOWERS_NUM 200
float flower_xAxis[FLOWERS_NUM];
float flower_yAxis[FLOWERS_NUM];
float flower_zAxis[FLOWERS_NUM];

LightSource whiteLS = {
	GL_LIGHT0, {1,1,1,1}, {1,1,1,1}, true
};

//
// APPLICATION DATA
//
Model tiles[6];
Model trees[4];

Texture flowerTexture;
Texture grassTexture;


Camera cam(0, 5, 20, 0, -10, .5);
bool drawMode = false;  /* F1 to toggle between LINE and FILL drawing */

//shape 6
Model horizontalRoad() {
	glRotatef(90, 0, 1, 0);
	return tiles[5];
}
//shape 7
Model shape2rotated90() {
	glRotatef(90, 0, 1, 0);
	return tiles[2];
}

//shape 8
Model shape4rotated90() {
	glRotatef(90, 0, 1, 0);
	return tiles[4];
}

//shape 9
Model shape1rotated180() {
	glRotatef(180, 0, 1, 0);
	return tiles[1];
}

//shape 10
Model shape2rotated270() {
	glRotatef(270, 0, 1, 0);
	return tiles[2];
}

void flowerSingle() {
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, flowerTexture.id);
	glBegin(GL_QUADS);

	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, 0);

	glTexCoord2f(1, 0);
	glVertex3f(1, -1, 0);

	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 0);

	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, 0);
	glEnd();
	glPopMatrix();
}

void flower() {
	glPushMatrix();
	flowerSingle();
	glRotatef(90, 0, 1, 0);
	flowerSingle();
	glPopMatrix();
}


//
// To display onto window using OpenGL commands
//
void display()
{  
    glClearColor(0.5, 0.6, 0.85, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	cam.LookAt();

	glPushMatrix();
	glTranslatef(0, 70, 0);
	whiteLS.addPointLS();
	glutSolidCube(3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(60, 70, 60);
	whiteLS.addPointLS();
	glutSolidCube(3);
	glPopMatrix();
	
	// Tiles, each tile dimension is 4x2x4 (x,y,z)
	for (int i = 0; i < MAP_SIZE; i++) {
		for (int j = 0; j < MAP_SIZE; j++) {
			if (trackMap[i][j] != 555) { //to avoid overlapping
				glPushMatrix();
				//translation for small tiles
				if (trackMap[i][j] == 0 || trackMap[i][j] == 2 ||
					trackMap[i][j] == 5 || trackMap[i][j] == 6 ||
					trackMap[i][j] == 7 || trackMap[i][j] == 10)
					glTranslatef(i * 8 - 60, -2, j * 8 - 10);
				//translation for big tiles
				else
					glTranslatef(i * 8 - 56, -2, j * 8 - 6);

				if (trackMap[i][j] == 6) drawTile = horizontalRoad();
				else if (trackMap[i][j] == 7) drawTile = shape2rotated90();
				else if (trackMap[i][j] == 8) drawTile = shape4rotated90();
				else if (trackMap[i][j] == 9) drawTile = shape1rotated180();
				else if (trackMap[i][j] == 10) drawTile = shape2rotated270();
				else drawTile = tiles[trackMap[i][j]];
				drawTile.render();
				glPopMatrix();
			}
		}
	}

	glPushMatrix();
	glTranslatef((3 * 8) - 60, -1, (5 * 8) - 10);
	glScalef(4.0, 4.0, 4.0);
	raceCar.render();
	glPopMatrix();


	for (int k = 0; k < TREES_NUM; k++) {
		glPushMatrix();
		glTranslatef(tree_xAxis[k], tree_yAxis[k], tree_zAxis[k]);
		glScalef(0.04, 0.04, 0.04);
		trees[tree_type[k]].render();
		glPopMatrix();
	}

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	for (int l = 0; l < FLOWERS_NUM; l++) {
		glPushMatrix();
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5);
		glTranslatef(flower_xAxis[l], flower_yAxis[l], flower_zAxis[l]);
		flower();
		glDisable(GL_ALPHA_TEST);
		glPopMatrix();
	}

	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeydown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	switch (key) {
	case 'w':
	case 'W': wKey = true; break;
	case 's':
	case 'S': sKey = true; break;
	case 'a':
	case 'A': aKey = true; break;
	case 'd':
	case 'D': dKey = true; break;
	case ' ': spaceKey = true; break;
	}
}

void onKeyup(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	switch (key) {
	case 'w':
	case 'W': wKey = false; break;
	case 's':
	case 'S': sKey = false; break;
	case 'a':
	case 'A': aKey = false; break;
	case 'd':
	case 'D': dKey = false; break;
	case ' ': spaceKey = false; break;
	}
}

void onSpecialKeydown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: upKey = true;	break;
	case GLUT_KEY_DOWN: downKey = true; break;
	case GLUT_KEY_LEFT: leftKey = true; break;
	case GLUT_KEY_RIGHT: rightKey = true; break;	
	}
}

void onSpecialKeyup(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: upKey = false;  break;
	case GLUT_KEY_DOWN: downKey = false; break;
	case GLUT_KEY_LEFT: leftKey = false; break;
	case GLUT_KEY_RIGHT: rightKey = false; break;
	case GLUT_KEY_F1: drawMode = !drawMode; 
		if (drawMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
}


void onClick(int button, int status, int x, int y)
{
	// Write your codes here.
	if (button == GLUT_MIDDLE_BUTTON && status == GLUT_DOWN) {
		capture = !capture;
		if (capture) {
			glutSetCursor(GLUT_CURSOR_NONE);
			glutWarpPointer(HALF_WIDTH, HALF_HEIGHT);
			mouse_x = HALF_WIDTH;
			mouse_y = HALF_HEIGHT;
		}
		else {
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
	}
	
}

void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, winWidth * 1.0f / winHeight, NEAR_PLANE, FAR_PLANE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0, 0, 0, 0);
	display(); // refresh window.
}

void onMove(int x, int y) {
	// Write your codes here.
	mouse_x = x;
	mouse_y = y;
}

void onTimer(int v) {
	
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	
	if (wKey) {
		cam.Forward();
	}
	if (sKey) {
		cam.Backward();
	}

	if (aKey) {
		cam.Left();
	}

	if (dKey) {
		cam.Right();
	}

			
	if (capture) {
		cam.TurnRightLeft((HALF_WIDTH - mouse_x) / MOUSE_SENSIVITY);
		cam.TurnUpDown((HALF_HEIGHT - mouse_y) / MOUSE_SENSIVITY);
		glutWarpPointer( HALF_WIDTH, HALF_HEIGHT);
	}
	
	glutPostRedisplay(); // display()

}

void initTrackMap() {
	//track tiles
	trackMap[3][4] = 7;
	trackMap[3][5] = 5; trackMap[3][6] = 5; trackMap[3][7] = 5; trackMap[3][8] = 5; trackMap[3][9] = 5;
	trackMap[3][10] = 9;
	trackMap[5][11] = 3;
	trackMap[7][12] = 6; trackMap[8][12] = 6;
	trackMap[9][11] = 4;
	trackMap[11][11] = 10;
	trackMap[11][10] = 5; trackMap[11][9] = 5; trackMap[11][8] = 5; trackMap[11][7] = 5; trackMap[11][6] = 5;
	trackMap[10][4] = 1;
	trackMap[4][4] = 6; trackMap[9][4] = 6;
	trackMap[5][4] = 6; trackMap[6][4] = 6; trackMap[7][4] = 6; trackMap[8][4] = 6;

	for (int i = 0; i < MAP_SIZE-1; i++) {
		for (int j = 0; j < MAP_SIZE-1; j++) {
			//for big tiles
			if (trackMap[i][j] == 1 || trackMap[i][j] == 3 ||
				trackMap[i][j] == 4 || trackMap[i][j] == 8 || trackMap[i][j] == 9 )
			{
				//clear overlapping grass
				trackMap[i + 1][j] = 555;
				trackMap[i][j+1] = 555;
				trackMap[i+1][j + 1] = 555;
			}
		}
	}
}

void initTreesAndFlowers() {
	srand(unsigned(time(NULL)));

	for (int i = 0; i < TREES_NUM; i++) {
		int thisTreeType = std::rand() % 4;
		tree_type[i] = thisTreeType;
		int tileLocI = int(std::rand() % MAP_SIZE);
		int tileLocJ = int(std::rand() % MAP_SIZE);
		int randomTile = trackMap[tileLocI][tileLocJ];
		while (randomTile!=0) {
			tileLocI = int(std::rand() % MAP_SIZE);
			tileLocJ = int(std::rand() % MAP_SIZE);
			randomTile = trackMap[tileLocI][tileLocJ];
		}
		
		tree_xAxis[i] = (tileLocI * 8.0 - 60.0) - 4.0 + (std::rand() % 8);
		tree_yAxis[i] = 5;
		tree_zAxis[i] = (tileLocJ * 8.0 - 10.0) - 4.0 + (std::rand() % 8);
	}

	for (int j = 0; j < FLOWERS_NUM; j++) {
		int tileLocI = int(std::rand() % MAP_SIZE);
		int tileLocJ = int(std::rand() % MAP_SIZE);
		int randomTile = trackMap[tileLocI][tileLocJ];
		while (randomTile!=0) {
			tileLocI = int(std::rand() % MAP_SIZE);
			tileLocJ = int(std::rand() % MAP_SIZE);
			randomTile = trackMap[tileLocI][tileLocJ];
		}
		
		flower_xAxis[j] = (tileLocI * 8.0 - 60.0) -4.0 + (std::rand() % 8);
		flower_yAxis[j] = 0;
		flower_zAxis[j] = (tileLocJ* 8.0 - 10.0) -4.0 +  (std::rand() % 8);
	}
}

void Init() {
	// Load Models
	tiles[0].init("models/racetrack", "terrain-grass.obj");
	tiles[1].init("models/racetrack", "track-corner-large.obj");
	tiles[2].init("models/racetrack", "track-corner-small.obj");
	tiles[3].init("models/racetrack", "track-left-right-large.obj");
	tiles[4].init("models/racetrack", "track-right-left-large.obj");
	tiles[5].init("models/racetrack", "track-straight-small.obj");

	trees[0].init("models/tree/tree1", "tree1.obj");
	trees[1].init("models/tree/tree2", "tree2.obj");
	trees[2].init("models/tree/tree3", "tree3.obj");
	trees[3].init("models/tree/tree4", "tree4.obj");

	whiteLS.init(); 
	flowerTexture.Load("texture/flower1.png");
	raceCar.init("models/car", "car-formula.obj");

	initTrackMap();
	initTreesAndFlowers();
	
	// Enable lighting and texturing
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE); // to normalize Normal vector for uniform scaling.
	//glEnable(GL_LIGHTING);

	// set the cursor position
	glutWarpPointer(HALF_WIDTH, HALF_HEIGHT);
	// hide cursor
	glutSetCursor(GLUT_CURSOR_NONE);
	
	glEnable(GL_DEPTH_TEST);	
}

void main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("CTIS489 - Midterm #2");
	
	Init();

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);
	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeydown);
	glutSpecialFunc(onSpecialKeydown);

	glutKeyboardUpFunc(onKeyup);
	glutSpecialUpFunc(onSpecialKeyup);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutPassiveMotionFunc(onMove);

	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	glutMainLoop();
}
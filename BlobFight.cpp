#include <GL/glut.h>
#include <string.h>
#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm> 

using namespace std;

/* Set initial display-window size. */
GLsizei winWidth = 1920, winHeight = 1080;

/* Set range for world coordinates. */
GLfloat xwcMin = 0.0, xwcMax = 1920.0;
GLfloat ywcMin = 0.0, ywcMax = 1080.0;

class Food 
{
public:
	int x, y, r;
};

double foodWhite[] = { 0.89, 0.91, 0.83 };
double foodGrey[] = { 0.55, 0.58, 0.66 };
double playerRed[] = { 0.94, 0.09, 0.2 };
double backGrey[] = { 0.21, 0.24, 0.27 };
double playerBlue[] = { 0.13, 0.64, 0.95 };

int tick = 0;
const int REFRESH_MS = 5;
int blueDirection = 0;
int redDirection = 0;

void displayFcn(void);
void bluePlayer(void);
void redPlayer(void);
void timer(int value);
void arrowFunc(int, int, int);
void keyboardFunc(unsigned char, int, int);
void init(void);
void winReshapeFcn(GLint newWidth, GLint newHeight);
void blueUpdatePos(void);
void redUpdatePos(void);
void foodBlob(int);
void populate(void);
void checkCollisionPlayers(void);
void updateBackground(void);
void updateSpeeds(void);
void textPrep(void);

double blueXPos = 150;
double blueYPos = 600;
double blueSpeed = 2;

double redXPos = 650;
double redYPos = 300;
double redSpeed = 2;

double blueRad = 40;
double redRad = 40;

const int NUM_BLOBS = 50;

int maxRad = max(redRad, blueRad);
	
Food foodArray[100];

void displayFcn(void) 
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	blueUpdatePos();
	glTranslatef(blueXPos, blueYPos, 0);
	bluePlayer();
	glPopMatrix();

	glPushMatrix();
	redUpdatePos();
	glTranslatef(redXPos, redYPos, 0);
	redPlayer();
	glPopMatrix();

	checkCollisionPlayers();
	updateBackground();
	updateSpeeds();

	for (int count = 0; count < NUM_BLOBS; count++) 
	{
		foodBlob(count);
	}
	textPrep();
    glFlush();
    tick++;
}

void drawBitmapText(char *string, float x, float y, float z, bool isBig) //prints bitmap text to screen
{
	char *c;
	glRasterPos3f(x, y, z);
	if (isBig) {
		for (c = string; *c != '\0'; c++)
		{
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
		}
	}
	else {
		for (c = string; *c != '\0'; c++)
		{
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
		}
	}
}

void updateSpeeds() //speed increases as players collect food, is based on other player's size.
{  
	blueSpeed = (redRad / 40);
	redSpeed = (blueRad / 40);
}

void textPrep() // prepares text to be printed to screen
{  
	if (tick < 1000) {
		glColor3f(1, 1, 1);
		drawBitmapText((char*)"BlobFight!", (xwcMax / 2) - 100, (ywcMax / 2) + 400, 0, true);
		drawBitmapText((char*)"Collect blobs to grow bigger and absorb the other player", (xwcMax / 2) - 180, (ywcMax / 2) + 360, 0, false);
	}

	if (redRad == 0) // checks if blue player won
	{ 
		drawBitmapText((char*)"The Blue Player Won!", (xwcMax / 2) - 180, (ywcMax / 2) + 300, 0, true);
	}
	else if (blueRad == 0) // checks if red player won
	{  
		drawBitmapText((char*)"The Red Player Won!", (xwcMax / 2) - 180, (ywcMax / 2) + 300, 0, true);
	}
}

void updateBackground() // changes color of background based on the player currently in the lead.
{ 
	if (blueRad > redRad) 
	{
		glClearColor(backGrey[0], backGrey[1], backGrey[2] + ((blueRad - redRad) / 100), 0.0);
	}
	else if (redRad > blueRad)
	{
		glClearColor(backGrey[0] + ((redRad - blueRad) / 100), backGrey[1], backGrey[2], 0.0);
	}
	else 
	{
		glClearColor(backGrey[0], backGrey[1], backGrey[2], 0.0);
	}
}

void checkCollisionPlayers() // checks to see if players are colliding
{
	if (sqrt((blueXPos - redXPos) * (blueXPos - redXPos) + (blueYPos - redYPos) * (blueYPos - redYPos)) < (redRad + blueRad - 10))
	{
		if (blueRad > redRad + 5) 
		{
			redRad = 0;
		}
		else if (redRad > blueRad + 5) 
		{
			blueRad = 0;
		}
	}
}

void checkCollisionFood(int foodX, int foodY, int foodRad, int count) // checks collisions from players to food  
{ 
	maxRad = max(redRad, blueRad);
	if (sqrt((foodX - redXPos) * (foodX - redXPos) + (foodY - redYPos) * (foodY - redYPos)) < (redRad + foodRad) && redRad < 500) 
	{
		redRad += foodRad / 10;
		foodArray[count].x = rand() % 1900 + 1;
		foodArray[count].y = rand() % 1000 + 1;
		foodArray[count].r = rand() % (maxRad / 2) + 1;
	}
	else if (sqrt((foodX - blueXPos) * (foodX - blueXPos) + (foodY - blueYPos) * (foodY - blueYPos)) < (blueRad + foodRad) && blueRad < 500) 
	{
		blueRad += foodRad / 10;
		foodArray[count].x = rand() % 1900 + 1;
		foodArray[count].y = rand() % 1000 + 1;
		foodArray[count].r = rand() % (maxRad / 2) + 1;
	}
}

void foodBlob(int count) // draws the foodBlob at count
{ 
	glPushMatrix();
	checkCollisionFood(foodArray[count].x, foodArray[count].y, foodArray[count].r, count);
	glTranslatef(foodArray[count].x, foodArray[count].y, 0);

	if (count % 2 == 0) 
	{
		glColor3f(foodGrey[1], foodGrey[2], foodGrey[3]);
	}
	else 
	{
		glColor3f(foodWhite[1], foodWhite[2], foodWhite[3]);
	}	

	int detail = 40;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0, 0);
		for (int i = 0; i < detail; i++) 
		{
			double deg = (i * (360) / detail);
			double x =  foodArray[count].r * cos(deg);
			double y = foodArray[count].r * sin(deg);
			glVertex2f(x, y);
		}
	glEnd();
	glPopMatrix();
}

void blueUpdatePos() // updates blue player's postion based on blueDirection case
{ 
	switch (blueDirection) 
	{
	case 0:
		if (blueXPos < xwcMax + 2)
			blueXPos += blueSpeed;
		break;
	case 1:
		if (blueYPos > -2)
			blueYPos -= blueSpeed;
		break;
	case 2:
		if (blueXPos > -2)
			blueXPos -= blueSpeed;
		break;
	case 3:
		if (blueYPos < ywcMax + 2)
			blueYPos += blueSpeed;
		break;
	};
}

void redUpdatePos() // updates red player's postion based on redDirection case
{ 
	switch (redDirection) {
	case 0:
		if (redXPos < xwcMax + 2)
			redXPos += redSpeed;
		break;
	case 1:
		if (redYPos > -2)
			redYPos -= redSpeed;
		break;
	case 2:
		if (redXPos > -2)
			redXPos -= redSpeed;
		break;
	case 3:
		if (redYPos < ywcMax + 2)
			redYPos += redSpeed;
		break;
	};
}

void bluePlayer() // draws blue player
{ 
    glPushMatrix();
	glRotatef(100 * sin(blueDirection), 0, 0, -1);
    glColor3f(playerBlue[0], playerBlue[1], playerBlue[2]);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    int detail = 30;
    for (int i = 0; i < detail; i++) 
	{
		double deg = (i * (360) / detail);
        double x = blueRad * cos(deg);
        double y = blueRad * sin(deg);
        glVertex2f(x, y);
    }
    glEnd();
    glPopMatrix();
}

void redPlayer() // draws red player
{ 
	glPushMatrix();
	glRotatef(100 * sin(redDirection), 0, 0, -1);
	glColor3f(playerRed[0], playerRed[1], playerRed[2]);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0, 0);
	int detail = 30;
	for (int i = 0; i < detail; i++) 
	{
		double deg = (i * (360) / detail);
		double x = redRad * cos(deg);
		double y = redRad * sin(deg);
		glVertex2f(x, y);
	}
	glEnd();
	glPopMatrix();
}

void keyboardFunc(unsigned char Key, int x, int y) //blue is controlled by wasd
{ 
	switch (Key)
	{
	case 'w':
		blueDirection = 3;
		break;
	case 'a':
		blueDirection = 2;
		break;
	case 's':
		blueDirection = 1;
		break;
	case 'd':
		blueDirection = 0;
		break;
	}
}

void arrowFunc(int key, int x, int y) //red is controlled by arrow keys
{ 
	switch (key) {
	case GLUT_KEY_UP:
		redDirection = 3;
		break;
	case GLUT_KEY_DOWN:
		redDirection = 1;
		break;
	case GLUT_KEY_LEFT:
		redDirection = 2;
		break;
	case GLUT_KEY_RIGHT:
		redDirection = 0;
		break;
	}
}

void populate() // for initial population of foodArray[]
{ 
	srand(time(NULL));
	for (int i = 0; i < NUM_BLOBS; i++) 
	{
		foodArray[i].x = rand() % 1900 + 1;
		foodArray[i].y = rand() % 1000 + 1;
		foodArray[i].r = rand() % (maxRad / 2) + 1;
	}
}

void winReshapeFcn(GLint newWidth, GLint newHeight)  // window reshape
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(xwcMin, xwcMax, ywcMin, ywcMax);
	glClear(GL_COLOR_BUFFER_BIT);
}

void timer(int value) 
{
	glutPostRedisplay();      // Post re-paint request to activate display()
	glutTimerFunc(REFRESH_MS, timer, 0); // next timer call milliseconds later
}

void init(void) //Set color of display window to a nice grey.
{ 
	glClearColor(backGrey[0], backGrey[1], backGrey[2], 0.0);
}

int main(int argc, char **argv)
{
	cout << "Loading new game...";
	populate();
	cout << "Done!";

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Blob Fight");
	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc(arrowFunc);

	init();

	glutDisplayFunc(displayFcn);
	glutReshapeFunc(winReshapeFcn);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
}
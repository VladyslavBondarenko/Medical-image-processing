#include <glut.h>
#include <iostream>
#include <sstream>
#include <iostream>
#include <math.h>
using std::string;
using std::ostringstream;

struct point {
	float x;
	float y;
	float z;
};

int rows = 512;
int columns = 512;
float rowSpacing = 0.782000;
float columnSpacing = 0.782000;
float sliceThickness = 3.00;
point target = { 287, 350 };
point imagePosition = { -204.102, -200.195, 1605.500 };
point imagePosition0 = { -204.102, -200.200, 1760.500 };
point rowOrientation = { 1, 0, 0 };
point columnOrientation = { 0, 1, 0 };
int imgNumber = 88;

point scalePoint(point p) {
	return {
		p.x / rows,
		p.y / rows,
		p.z / rows
	};
}
point toPixels(point p) {
	return {
		p.x / rowSpacing,
		p.y / columnSpacing,
		p.z / sliceThickness
	};
}
point toMM(point p) {
	return {
		p.x * rowSpacing,
		p.y * columnSpacing,
		p.z * sliceThickness
	};
}
void glutPrint(float x, float y, float z, void* font, string text) {
	glColor3f(0, 0, 0);
	glRasterPos3f(x, y, z);
	for (char& c : text) {
		glutBitmapCharacter(font, (int)c);
	}
}

void drawVolume(point p0, point p1) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	p0 = scalePoint(p0);
	p1 = scalePoint(p1);
	glColor3f(0, 0, 0);
		glVertex3f(p1.x, p1.y, p0.z); glVertex3f(p0.x, p1.y, p0.z); glVertex3f(p0.x, p1.y, p1.z); glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p1.x, p0.y, p1.z); glVertex3f(p0.x, p0.y, p1.z); glVertex3f(p0.x, p0.y, p0.z); glVertex3f(p1.x, p0.y, p0.z);
		glVertex3f(p1.x, p1.y, p1.z); glVertex3f(p0.x, p1.y, p1.z); glVertex3f(p0.x, p0.y, p1.z); glVertex3f(p1.x, p0.y, p1.z);
		glVertex3f(p1.x, p0.y, p0.z); glVertex3f(p0.x, p0.y, p0.z); glVertex3f(p0.x, p1.y, p0.z); glVertex3f(p1.x, p1.y, p0.z);
		glVertex3f(p0.x, p1.y, p1.z); glVertex3f(p0.x, p1.y, p0.z); glVertex3f(p0.x, p0.y, p0.z); glVertex3f(p0.x, p0.y, p1.z);
		glVertex3f(p1.x, p1.y, p0.z); glVertex3f(p1.x, p1.y, p1.z); glVertex3f(p1.x, p0.y, p1.z); glVertex3f(p1.x, p0.y, p0.z);
	glEnd();
}
void drawImg(point p0, point p1) {
	p0 = scalePoint(p0);
	p1 = scalePoint(p1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_QUADS);
	glVertex3f(p0.x, p0.y, p0.z);
	glVertex3f(p0.x, p1.y, p0.z);
	glVertex3f(p1.x, p1.y, p0.z);
	glVertex3f(p1.x, p0.y, p0.z);
	glEnd();
}
void drawAxes() {
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(5, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 5, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 5);
	glEnd();
	glutPrint(3, 0, 0, GLUT_BITMAP_9_BY_15, "X");
	glutPrint(0, 3, 0, GLUT_BITMAP_9_BY_15, "Y");
	glutPrint(0, 0, 3, GLUT_BITMAP_9_BY_15, "Z");
}
void drawPoint(point p) {
	p = scalePoint(p);
	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex3f(p.x, p.y, p.z);
	glEnd();
}

void printResult(point p) {
	glPopMatrix();
	ostringstream valuesPx;
	ostringstream valuesMm;
	valuesPx << (int)p.x << " " << (int)p.y << " " << (int)p.z;
	valuesMm << toMM(p).x << " " << toMM(p).y << " " << toMM(p).z;
	string textPx = "coordinates px: " + valuesPx.str();
	string textMm = "coordinates mm: " + valuesMm.str();
	glutPrint(-1, -1, 0, GLUT_BITMAP_9_BY_15, textPx);
	glutPrint(-1, -0.8, 0, GLUT_BITMAP_9_BY_15, textMm);
}

void printElements() {
	point imagePositionPixels = toPixels(imagePosition);
	point imagePosition0Pixels = toPixels(imagePosition0);
	point lastPixelPosition = {
		rowOrientation.x * rows + imagePosition0Pixels.x,
		columnOrientation.y * columns + imagePosition0Pixels.y,
		(signbit(imagePositionPixels.z - imagePosition0Pixels.z) ? -sliceThickness : sliceThickness) * imgNumber + imagePosition0Pixels.z
	};
	point targetPixel = {
		target.x + imagePositionPixels.x,
		target.y + imagePositionPixels.y,
		imagePositionPixels.z
	};

	drawImg(imagePositionPixels, lastPixelPosition);
	drawVolume(imagePosition0Pixels, lastPixelPosition);
	glColor3f(1, 0, 0);
	drawPoint(targetPixel);
	drawAxes();
	printResult(targetPixel);
}

class RenderState {
public:
	float mouseX, mouseY, cameraX, cameraY;
	bool mouseLeftDown, mouseRightDown;
	RenderState() {
		this->mouseX = 0;
		this->mouseY = 0;
		this->mouseLeftDown = false;
		this->mouseRightDown = false;
		this->cameraX = 0.0f;
		this->cameraY = 0.0f;
	}
};

RenderState rs;

void display() {
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(600.0, rows / columns, 0.5, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.0, -0.5, -3);
	glPushMatrix();

	glRotatef(rs.cameraX, 1, 0, 0);
	glRotatef(rs.cameraY, 0, 1, 0);

	glFlush();
	printElements();
	glutSwapBuffers();
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	RenderState* rsp = new RenderState();
	rs = *rsp;
}

void exit() {
	delete &rs;
}

void mouseCallback(int button, int state, int x, int y)
{
	rs.mouseX = x;
	rs.mouseY = y;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			rs.mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			rs.mouseLeftDown = false;
	}
}

void mouseMotionCallback(int x, int y)
{
	if (rs.mouseLeftDown)
	{
		rs.cameraY += (x - rs.mouseX);
		rs.cameraX += (y - rs.mouseY);
		rs.mouseX = x;
		rs.mouseY = y;
	}
}


void idleCallback()
{
	glutPostRedisplay();
}



int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(columns * 2, rows * 2);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Bondarenko RGR");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(idleCallback);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMotionCallback);
	glViewport(0, 0, rows*2, columns*2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glutMainLoop();
	exit();
	return 0;
}
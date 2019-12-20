#include <glut.h>
#include <imebra/imebra.h>
#include <string.h>
#include <iostream>

using namespace std;
using namespace imebra;

GLuint texID = 0;
int WIDTH = 0;
int HEIGHT = 0;
double image_position[2];
double image_orientation[6];
string patient_position;
string patient_orientation[3];
double pixel_spacing = 0.4;
int pX, pY;
double x_pos, y_pos, z_pos;

DataSet* loadedDataSet;
size_t dataLength;

unsigned char* buffer;

string DATATYPE;

void printText(string str, int x, int y, float r = 0, float g = 1, float b = 0 )
{
	glColor3f(r, g, b);
	glRasterPos2f(x, y);

	for (int i = 0; i < str.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
	}
}

void printCoordinates() {
	printText("px: " + to_string(pX) + " " + to_string(pY), -WIDTH / 2 + 20, HEIGHT / 2 - 20);
	printText("mm: " + to_string(x_pos) + " " + to_string(y_pos) + " " + to_string(z_pos), -WIDTH / 2 + 20, HEIGHT / 2 - 40);
	printText("L", WIDTH / 2 - 5, -HEIGHT / 4);
	printText("A", WIDTH / 4, -HEIGHT / 2 + 5);
}

void initializeWindow()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH * 2, HEIGHT * 2);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Lab 5");
}

void readPixelsData(string filename, int frameNumber = 0) {
	loadedDataSet = CodecFactory::load(filename);
	Image* image(loadedDataSet->getImage(frameNumber));
	WIDTH = image->getWidth();
	HEIGHT = image->getHeight();
	ReadingDataHandlerNumeric* dataHandler = image->getReadingDataHandler();
	buffer = (unsigned char*)dataHandler->data(&dataLength);
}

void readImage()
{
	string filename = "dicom.dcm";
	readPixelsData(filename);
	for (int i = 0; i <= 5; i++) {
		image_orientation[i] = loadedDataSet->getDouble(TagId(0x0020, 0x0037), i);
	}
	for (int i = 0; i <= 1; i++) {
		image_position[i] = loadedDataSet->getDouble(TagId(0x0020, 0x0032), i);
		patient_orientation[i] = loadedDataSet->getString(TagId(0x0020, 0x0020), i);
	}
	//pixel_spacing = loadedDataSet->getDouble(TagId(0x0028, 0x0030), 0);
}

void drawImage() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 1.0);
	glColor3f(1, 1, 1);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, WIDTH, HEIGHT, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(-WIDTH/2, WIDTH/2, -HEIGHT/2, HEIGHT/2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texID);

	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex2f(0, 0);
	glTexCoord2f(1, 0);
	glVertex2f(0, -HEIGHT / 2);
	glTexCoord2f(0, 0);
	glVertex2f(WIDTH / 2, -HEIGHT / 2);
	glTexCoord2f(0, 1);

	glVertex2f(WIDTH / 2, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	printCoordinates();

	glFlush();
	glutSwapBuffers();
}

void mouseMotionCallback(int x, int y)
{
	pX = x - WIDTH;
	pY = HEIGHT - y;

	if (WIDTH / 2 > pX > 0 && 0 > pY > -HEIGHT / 2 ) {
		x_pos = image_position[0] + pixel_spacing * image_orientation[0] * pX + pixel_spacing * image_orientation[3] * pY;
		y_pos = image_position[1] + pixel_spacing * image_orientation[4] * pY + pixel_spacing * image_orientation[1] * pX;
		z_pos = image_position[2] + image_orientation[2] * pX / image_position[2] + image_orientation[5] * pY / image_position[2];
	}
}
void idleCallback()
{
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	readImage();
	initializeWindow();
	glutDisplayFunc(drawImage);
	glutIdleFunc(idleCallback);
	glutMotionFunc(mouseMotionCallback);
	glutMainLoop();
	return 0;
}

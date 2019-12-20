#include <glut.h>
#include <imebra/imebra.h>
#include <string.h>
#include <iostream>

using namespace std;
using namespace imebra;

GLuint texID = 0;
string text = "Patient's Weight: ";
int WIDTH = 0;
int HEIGHT = 0;
unsigned char* buffer;

void printText(string str, int x, int y, float r = 1, float g = 1, float b = 1 )
{
	glColor3f(r, g, b);
	glRasterPos2f(x, y);

	for (int i = 0; i < str.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, str[i]);
	}
}

void display(void)
{
	bool showText = false;
	cout << "Type 1 to show/hide text: ";

	do {
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0, 0, 0, 1.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluOrtho2D(0, WIDTH, 0, HEIGHT);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texID);

		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(0, 0);
			glTexCoord2f(0, 1);
			glVertex2f(0, HEIGHT);
			glTexCoord2f(1, 1);
			glVertex2f(WIDTH, HEIGHT);
			glTexCoord2f(1, 0);
			glVertex2f(WIDTH, 0);
		glEnd();

		glDisable(GL_TEXTURE_2D);

		const int textPositionX = 0;
		const int textPositionY = HEIGHT - 15;

		glLoadIdentity();
		gluOrtho2D(0, WIDTH, 0, HEIGHT);

		if (showText) {
			printText(text, textPositionX, textPositionY);
		}

		glFlush();
		glutSwapBuffers();

		int k;
		cin >> k;
		showText = !showText;

	} while (true);
}

void displayImage()
{
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, WIDTH, HEIGHT, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glutDisplayFunc(display);
}

void initializeWindow()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Lab 3");
}

void readImage()
{
	const int frameNumber = 0;
	const string DICOM_FILE_PATH = "dicom.dcm";

	DataSet* loadedDataSet(CodecFactory::load(DICOM_FILE_PATH));
	Image* image(loadedDataSet->getImage(frameNumber));

	string colorSpace = image->getColorSpace();

	string weightValue = loadedDataSet->getString(TagId(0x0010, 0x1030), 0);
	text += weightValue;

	WIDTH = image->getWidth();
	HEIGHT = image->getHeight();

	ReadingDataHandlerNumeric* dataHandler = image->getReadingDataHandler();

	size_t dataLength;
	const char* data = dataHandler->data(&dataLength);

	buffer = new unsigned char[dataLength];
	memcpy(buffer, data, dataLength);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	readImage();
	initializeWindow();
	displayImage();
	glutMainLoop();
	return 0;
}

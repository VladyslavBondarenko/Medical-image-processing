#include <glut.h>
#include <imebra/imebra.h>
#include <string.h>
#include <iostream>

using namespace std;
using namespace imebra;

GLuint texID = 0;
int WIDTH = 0;
int HEIGHT = 0;
size_t dataLength;
bool transformed = false;
DataSet* loadedDataSet;
DataSet* newDataSet;

unsigned char* buffer;
float* newBuffer;
size_t min, max;
float newMin, newMax;

double SLOPE, INTERCEPT;
string DATATYPE;

float slope = 0.0101769;
float intercept = 3.142;
int upperBound = 250;
int lowerBound = 85;

void printText(string str, int x, int y, float r = 0, float g = 1, float b = 0 )
{
	glColor3f(r, g, b);
	glRasterPos2f(x, y);

	for (size_t i = 0; i < str.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, str[i]);
	}
}

void initializeWindow()
{
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClear(GL_COLOR_BUFFER_BIT);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Lab 4");
}

size_t getMin(const unsigned char* buffer)
{
	size_t min = buffer[0];
	for (size_t i = 1; i <= dataLength; i++)
	{
		if (buffer[i] < min)
		{
			min = buffer[i];
		}
	}
	return min;
}

float getMin(const float* buffer)
{
	float min = buffer[0];
	for (size_t i = 1; i <= dataLength; i++)
	{
		if (buffer[i] < min)
		{
			min = buffer[i];
		}
	}
	return min;
}

size_t getMax(const unsigned char *buffer)
{
	size_t max = buffer[0];
	for (size_t i = 1; i <= dataLength; i++)
	{
		if (buffer[i] > max)
		{
			max = buffer[i];
		}
	}
	return max;
}

float getMax(const float *buffer)
{
	float max = buffer[0];
	for (size_t i = 1; i <= dataLength; i++)
	{
		if (buffer[i] > max)
		{
			max = buffer[i];
		}
	}
	return max;
}

void makePixelOperations(unsigned char* input, float* output)
{
	for (int i = 0; i <= dataLength; i++)
	{
		if (input[i] < lowerBound)
			output[i] = intercept + slope * lowerBound;
		else if (input[i] > upperBound)
			output[i] = intercept + slope * upperBound;
		output[i] = intercept + slope * input[i];
	}
	newMin = getMin(output);
	newMax = getMax(output);
	for (size_t i = 0; i < dataLength; ++i)
	{
		output[i] = (output[i] - newMin) / newMax;
	}
}

void printDicomInfo(DataSet* dataSet)
{
	const int textPositionX = 0;
	const int lineHeight = 15;
	int textPositionY = HEIGHT - 15;

	int bits = stoi(dataSet->getString(TagId(0x0028, 0x0100), 0));
	DATATYPE = (bits == 8) ? "GL_UNSIGNED_BYTE" : "GL_FLOAT";
	printText("DATATYPE: " + DATATYPE, textPositionX, textPositionY);
	textPositionY -= lineHeight;

	if (dataSet->bufferExists(TagId(0x0008, 0x0008), 0)) {
		INTERCEPT = dataSet->getDouble(TagId(0x0028, 0x1052), 0);
		printText("INTERCEPT: " + to_string(INTERCEPT), textPositionX, textPositionY);
		textPositionY -= lineHeight;
	}
	if (dataSet->bufferExists(TagId(0x0008, 0x0008), 0)) {
		SLOPE = dataSet->getDouble(TagId(0x0028, 0x1053), 0);
		printText("SLOPE: " + to_string(SLOPE), textPositionX, textPositionY);
		textPositionY -= lineHeight;
	}

	string minString = transformed ? to_string(newMin) : to_string(min);
	string maxString = transformed ? to_string(newMax) : to_string(max);
	printText("Min: " + minString, textPositionX, textPositionY);
	textPositionY -= lineHeight;
	printText("Max: " + maxString, textPositionX, textPositionY);
	textPositionY -= lineHeight;


}

char* genRandom(const int len) {
	char* s = new char[len];
	for (int i = 0; i < len; ++i) {
		s[i] = rand() % 10;
	}
	return s;
}

void readPixelsData(string filename, int frameNumber = 0) {
	loadedDataSet = CodecFactory::load(filename);
	newDataSet = CodecFactory::load(filename);
	Image* image(loadedDataSet->getImage(frameNumber));
	WIDTH = image->getWidth();
	HEIGHT = image->getHeight();
	ReadingDataHandlerNumeric* dataHandler = image->getReadingDataHandler();
	const char* data = dataHandler->data(&dataLength);
	buffer = new unsigned char[dataLength];
	memcpy(buffer, data, dataLength);
	max = getMax(buffer);
	min = getMin(buffer);
}

void readImage()
{
	string filename;
	cout << "Filename: ";
	cin >> filename;
	readPixelsData(filename);
}

void transform() {
	newDataSet->setString(TagId(0x0008, 0x0008),"DERIVED");
	//newDataSet->setString(TagId(0x0008, 0x103E), "NEW DESCRIPTION");
	if (newDataSet->bufferExists(imebra::TagId(0x0008, 0x103F), 0))
	{
		newDataSet->setString(imebra::TagId(0x0008, 0x103F), "");
	}
	if (!newDataSet->bufferExists(imebra::TagId(0x0020, 0x000E), 0))
	{
		newDataSet->setString(imebra::TagId(0x0020, 0x000E), genRandom(64));
	}
	if (newDataSet->bufferExists(imebra::TagId(0x0200, 0x0011), 0))
	{
		newDataSet->setSignedLong(imebra::TagId(0x0200, 0x0011), 1);
	}
	if (newDataSet->bufferExists(imebra::TagId(0x0020, 0x0013), 0))
	{
		newDataSet->setSignedLong(imebra::TagId(0x0020, 0x0013), 1);
	}
	char* SOP_UI = genRandom(64);
	if (newDataSet->bufferExists(imebra::TagId(0x0020, 0x0016), 0))
	{
		newDataSet->setString(imebra::TagId(0x0020, 0x0016), SOP_UI);
	}
	if (newDataSet->bufferExists(imebra::TagId(0x0002, 0x0003), 0))
	{
		newDataSet->setString(imebra::TagId(0x0002, 0x0003), SOP_UI);
	}
	newDataSet->setUnsignedLong(TagId(0x0028, 0x0100), 64);
	newDataSet->setDouble(TagId(0x0028, 0x1052), intercept);
	newDataSet->setDouble(TagId(0x0028, 0x1053), slope);
	newBuffer = new float[dataLength];
	makePixelOperations(buffer, newBuffer);
}

void editPixelsData()
{
	const int frameNumber = 0;
	Image* newImage(newDataSet->getImage(frameNumber));
	WritingDataHandlerNumeric* writingDataHandler = newImage->getWritingDataHandler();
	char* data = writingDataHandler->getMemory()->data(&dataLength);
	for (int i = 0; i <= dataLength; i++)
	{
		data[i] = intercept + slope * data[i];
		if (data[i] < lowerBound)
			data[i] = lowerBound;
		else if (data[i] > upperBound)
			data[i] = upperBound;
	}
}

void saveFile() {
	string fileNameSave;
	cout << "File name: ";
	cin >> fileNameSave;

	if (transformed) {
		CodecFactory::save(*loadedDataSet, fileNameSave, codecType_t::dicom);
	}
	else {
		editPixelsData();
		CodecFactory::save(*newDataSet, fileNameSave, codecType_t::dicom);
	}
	cout << "saved" << endl << endl;
}

void drawImage(DataSet* dataSet) {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 1.0);
	glColor3f(1, 1, 1);

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	if (transformed) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, WIDTH, HEIGHT, 0, GL_LUMINANCE, GL_FLOAT, newBuffer);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, WIDTH, HEIGHT, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

	printDicomInfo(dataSet);

	glFlush();
	glutSwapBuffers();
}

void display(void)
{
	drawImage(loadedDataSet);

	DataSet* dataSet;
	do {
		int choice;
		if (transformed) {
			cout << "Transform back (1)" << endl;
			dataSet = newDataSet;
		}
		else {
			cout << "Transform (1)" << endl;
			dataSet = loadedDataSet;
		}
		cout << "Save to file (2)" << endl;
		cout << "Load image (3)" << endl;
		cin >> choice;

		switch (choice)
		{
		case 1:
		{
			transform();
			drawImage(dataSet);
			transformed = !transformed;
			break;
		}
		case 2:
		{
			saveFile();
			break;
		}
		case 3:
		{
			readImage();
			drawImage(dataSet);
			break;
		}
		default:
			cout << "Try again" << endl << endl;
		}
		cout << endl;
	} while (true);
}

void displayImage()
{
	readImage();
	initializeWindow();
	glutDisplayFunc(display);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	displayImage();
	glutMainLoop();
	return 0;
}

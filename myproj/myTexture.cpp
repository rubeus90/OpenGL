#include "StdAfx.h"
#include "myTexture.h"

bool myTexture::readTexture(char *filename)
{ 
    FILE *inFile; 
	char buffer[100]; 
    GLubyte *mytexture; 
	unsigned char c; 
	int maxVal;

	if( (inFile = fopen(filename, "rb")) == NULL) {
		return 0;
	}

	//Read file type identifier (magic number)
	fgets(buffer, sizeof(buffer), inFile);
	if ((buffer[0] != 'P') || (buffer[1] != '6')) {
		fprintf (stderr, "not a binary ppm file %s\n", filename);
		return 0;
	}

	if(buffer[2] == 'A')
		pixelsize = 4;
	else
		pixelsize = 3;

	//Read image size
	do fgets(buffer, sizeof (buffer), inFile);
	while (buffer[0] == '#');
	sscanf (buffer, "%d %d", &width, &height);

	//Read maximum pixel value (usually 255)
	do fgets (buffer, sizeof (buffer), inFile);
	while (buffer[0] == '#');
	sscanf (buffer, "%d", &maxVal);

	//Allocate RGBA texture buffer
	int memSize = width * height * 4 * sizeof(GLubyte);
	mytexture = new GLubyte[memSize];

	// read RGB data and set alpha value
	for (int i = 0; i < memSize; i++) {
		if ((i % 4) < 3 || pixelsize == 4) {
			c = fgetc(inFile);
			mytexture[i]=(GLubyte) c;
        }
		else mytexture[i] = (GLubyte) 255; //Set alpha to opaque
    }
    fclose(inFile);

	glGenTextures(1, &texName) ; 
    glBindTexture (GL_TEXTURE_2D, texName) ; 

    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ; 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ; 
   
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLuint)width, (GLuint)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mytexture);

	delete[] mytexture;
}

GLubyte* myTexture::readImage(char* filename){
	FILE *inFile;
	char buffer[100];
	GLubyte *mytexture;
	unsigned char c;
	int maxVal, width, height, pixelsize;

	if ((inFile = fopen(filename, "rb")) == NULL) {
		return 0;
	}

	//Read file type identifier (magic number)
	fgets(buffer, sizeof(buffer), inFile);
	if ((buffer[0] != 'P') || (buffer[1] != '6')) {
		fprintf(stderr, "not a binary ppm file %s\n", filename);
		return 0;
	}

	if (buffer[2] == 'A')
		pixelsize = 4;
	else
		pixelsize = 3;

	//Read image size
	do fgets(buffer, sizeof(buffer), inFile);
	while (buffer[0] == '#');
	sscanf(buffer, "%d %d", &width, &height);

	//Read maximum pixel value (usually 255)
	do fgets(buffer, sizeof(buffer), inFile);
	while (buffer[0] == '#');
	sscanf(buffer, "%d", &maxVal);

	//Allocate RGBA texture buffer
	int memSize = width * height * 4 * sizeof(GLubyte);
	mytexture = new GLubyte[memSize];

	// read RGB data and set alpha value
	for (int i = 0; i < memSize; i++) {
		if ((i % 4) < 3 || pixelsize == 4) {
			c = fgetc(inFile);
			mytexture[i] = (GLubyte)c;
		}
		else mytexture[i] = (GLubyte)255; //Set alpha to opaque
	}
	fclose(inFile);

	return mytexture;
}

void myTexture::readCubeMapping(){
	GLuint cubemap_texture;
	GLubyte* cubeMap[6];
	cubeMap[0] = readImage("1.ppm");
	cubeMap[1] = readImage("2.ppm");
	cubeMap[2] = readImage("3.ppm");
	cubeMap[3] = readImage("4.ppm");
	cubeMap[4] = readImage("5.ppm");
	cubeMap[5] = readImage("6.ppm");

	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texName);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	for (int i = 0; i<6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, cubeMap[i]);
	}
}
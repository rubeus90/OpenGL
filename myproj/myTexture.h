#pragma once
#include <GL/glew.h>

class myTexture
{
public:
	int width, height, pixelsize;
	GLuint texName;

	bool readTexture(char *filename);
	GLubyte* myTexture::readImage(char* filename);
	void readCubeMapping();
	void myTexture::createReflection(GLuint* fboId, int Glut_w, int Glut_h);
};


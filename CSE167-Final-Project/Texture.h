#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <SOIL.h>
#include <iostream>

class Texture
{
private:
	GLuint texture;
	int width;
	int height;

	void generateTexture(const char* filename);

public:
	Texture(const char* filename);
	~Texture();

	void setTexture(const char* filename);
	GLuint getTexture() { return this->texture; }
};

#endif
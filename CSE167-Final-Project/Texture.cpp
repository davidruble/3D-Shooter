#include "Texture.h"

Texture::Texture(const char* filename)
{
	generateTexture(filename);
}

Texture::~Texture()
{

}

void Texture::setTexture(const char* filename)
{
	generateTexture(filename);
}

void Texture::generateTexture(const char* filename)
{
	//generate the texture object
	glGenTextures(1, &texture);
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	//set the wrapping/filerting options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	//load the image and bind it to this texture
	unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
	if (image == 0)
	{
		std::cerr << "ERROR: Unable to load image file: " << filename << std::endl;
		return;
	}
	else
	{
		std::cerr << "Texture loaded!" << std::endl;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

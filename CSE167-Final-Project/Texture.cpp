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
	glBindTexture(GL_TEXTURE_2D, texture);

	//set the wrapping/filerting options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0f);

	//use anisotropic filtering, if available
	float amount = 0.0f;
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &amount);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
	}
	else
	{
		std::cerr << "Anisotropoic filtering not supported" << std::endl;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.5f);
	}
	
	//load the image and bind it to this texture
	unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
	if (image == 0)
	{
		std::cerr << "ERROR: Unable to load image file: " << filename << std::endl;
		return;
	}
	else
	{
		//std::cerr << "Texture loaded!" << std::endl;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

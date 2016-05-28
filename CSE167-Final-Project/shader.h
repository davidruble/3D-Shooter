#ifndef SHADER_HPP
#define SHADER_HPP

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

class Shader
{
private:
	GLuint shaderProgram;
	void LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

public:
	Shader() {}
	Shader(const char * vertex_file_path,const char * fragment_file_path);
	~Shader();

	void use() { glUseProgram(shaderProgram); }
	GLuint getProgram() { return shaderProgram; }
};

#endif

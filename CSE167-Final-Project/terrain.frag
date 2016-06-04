#version 330 core

in vec2 pass_texCoords;

out vec4 color;

uniform sampler2D modelTexture;

void main()
{
	color = texture(modelTexture, pass_texCoords);
}
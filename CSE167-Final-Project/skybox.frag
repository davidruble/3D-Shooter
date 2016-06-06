#version 330 core

in vec3 TexCoords;
in float visibility;

out vec4 color;

uniform samplerCube skybox;
uniform vec3 skyColor;

void main()
{
	color = texture(skybox, TexCoords);
	color = mix(vec4(skyColor, 1.0), color, visibility);
}
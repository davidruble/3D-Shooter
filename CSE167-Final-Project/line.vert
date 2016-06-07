#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 worldPos = vec4(position, 1.0);
	vec4 positionRelativeToCam = view * worldPos;
	gl_Position = projection * positionRelativeToCam;
}

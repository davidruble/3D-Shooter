#version 330 core

layout (location = 0) in vec3 vertices;
layout (location = 1) in vec4 center;
layout (location = 2) in vec4 color;

out vec4 particleColor;

uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform mat4 ViewProj;

void main()
{
	float particleSize = center.w;
	vec3 particleCenter = center.xyz;

	vec3 position = 
		particleCenter + 
		(cameraRight * vertices.x * particleSize) +
		(cameraUp * vertices.y * particleSize);

	gl_Position = ViewProj * vec4(position, 1.0f);
	particleColor = color;
}
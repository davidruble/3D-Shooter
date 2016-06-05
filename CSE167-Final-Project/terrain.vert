#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec2 pass_texCoords;
out vec3 surfaceNormal;
out vec3 toLight;
out vec3 toCamera;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;

void main()
{
	vec4 worldPos = model * vec4(position, 1.0);
	gl_Position = projection * view * worldPos;
	pass_texCoords = texCoords * 100.0;

	surfaceNormal = (model * vec4(normal, 0.0)).xyz;
	toLight = lightPosition - worldPos.xyz;
	toCamera = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPos.xyz;
}
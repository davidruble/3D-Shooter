#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;

out vec3 Normal;
out vec3 FragPos;
out vec3 surfaceNormal;
out vec3 toLight;
out vec3 toCamera;

out vec3 TexCoords;

void main()
{
	vec4 worldPos = model * vec4(position, 1.0);
	vec4 positionRelativeToCam = view * worldPos;
	gl_Position = projection * positionRelativeToCam;

	surfaceNormal = (model * vec4(normal, 0.0)).xyz;
	toLight = lightPosition - worldPos.xyz;
	toCamera = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPos.xyz;

	TexCoords = position;
}
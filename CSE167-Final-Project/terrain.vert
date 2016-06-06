#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec2 pass_texCoords;
out vec3 surfaceNormal;
out vec3 toLight;
out vec3 toCamera;
out float elevation;
out float visibility;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPosition;
uniform float fogDensity;
uniform float fogGradient;

float rand(float n)
{
	return fract(sin(n) * 43758.5453123);
}

float add_noise(float p)
{
	float fl = floor(p);
	float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

void main()
{
	vec4 worldPos = model * vec4(position, 1.0);
	vec4 positionRelativeToCam = view * worldPos;
	gl_Position = projection * positionRelativeToCam;
	pass_texCoords = texCoords;

	elevation = worldPos.y + add_noise(worldPos.y) * 2.5;
	surfaceNormal = (model * vec4(normal, 0.0)).xyz;
	toLight = lightPosition - worldPos.xyz;
	toCamera = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPos.xyz;

	//fog stuff
	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance * fogDensity), fogGradient));
	visibility = clamp(visibility, 0.0, 1.0);
}

#version 330 core

layout (location = 0) in vec3 position;
out vec3 TexCoords;
out float visibility;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float fogDensity;
uniform float fogGradient;

void main()
{
	vec4 worldPos = model * vec4(position, 1.0);
	vec4 positionRelativeToCam = view * worldPos;
	vec4 pos = projection * positionRelativeToCam;
	gl_Position = pos.xyww;
	TexCoords = position;

	//fog stuff
	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow((distance * fogDensity), fogGradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
#version 330 core

in vec2 pass_texCoords;
in vec3 surfaceNormal;
in vec3 toLight;
in vec3 toCamera;

out vec4 color;

uniform sampler2D modelTexture;
uniform vec3 lightColor;
uniform float shineDamper;
uniform float reflectivity;

void main()
{
	vec3 unitNormal = normalize(surfaceNormal);
	vec3 unitLightVector = normalize(toLight);
	
	float nDotl = dot(unitNormal,unitLightVector);
	float brightness = max(nDotl,0.2);
	vec3 diffuse = brightness * lightColor;
	
	vec3 unitVectorToCamera = normalize(toCamera);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection,unitNormal);
	
	float specularFactor = dot(reflectedLightDirection , unitVectorToCamera);
	specularFactor = max(specularFactor,0.0);
	float dampedFactor = pow(specularFactor,shineDamper);
	vec3 finalSpecular = dampedFactor * reflectivity * lightColor;
	
	color = vec4(diffuse,1.0) * texture(modelTexture, pass_texCoords) + vec4(finalSpecular,1.0);
	
	//color = texture(modelTexture, pass_texCoords);
}
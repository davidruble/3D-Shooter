#version 330 core

in vec2 pass_texCoords;
in vec3 surfaceNormal;
in vec3 toLight;
in vec3 toCamera;
in float elevation;
in float visibility;

out vec4 color;

uniform sampler2D highTexture;
uniform sampler2D midTexture;
uniform sampler2D lowTexture;

uniform vec3 lightColor;
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColor;

void main()
{
	vec2 tiledCoords = pass_texCoords * 75.0;
	vec4 highTexColor = texture(highTexture, tiledCoords);
	vec4 midTexColor = texture(midTexture, tiledCoords);
	vec4 lowTexColor = texture(lowTexture, tiledCoords);

	//set the texture based on elevation
	vec4 finalColor = vec4(0.0, 0.0, 0.0, 0.0);
	if (elevation <= -10.0)
		finalColor += lowTexColor;
	if (elevation >= -10.0 && elevation <= -9.5)
		finalColor += (lowTexColor + midTexColor) / 2.0;
	if (elevation >= -9.5 && elevation <= 1.8)
		finalColor += midTexColor;
	//if (elevation >= 1.0 && elevation <= 1.8)
	//	finalColor += (midTexColor + highTexColor) / 2.0;
	if (elevation >= 1.8)
		finalColor += highTexColor + 0.2 * midTexColor;

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
	
	color = vec4(diffuse,1.0) * finalColor + vec4(finalSpecular,1.0);
	color = mix(vec4(skyColor, 1.0), color, visibility);
}
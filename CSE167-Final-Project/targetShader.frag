#version 330 core
in vec3 surfaceNormal;
in vec3 toLight;
in vec3 toCamera;
in float elevation;
in float visibility;

out vec4 color;

uniform vec3 lightColor;
uniform float shineDamper;
uniform float reflectivity;

void main()
{             
	vec4 finalColor = vec4(1.0, 1.0, 0, 1);
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
	color = mix(vec4(1, 0, 0, 1.0), color, visibility);

	float edge = max(0, dot(unitNormal, toCamera));
		if( edge < 0.5f){
	color = vec4(0, 0, 1, edge);
	}
}
#version 330 core
in vec3 surfaceNormal;
in vec3 toLight;
in vec3 toCamera;
in float elevation;
in float visibility;
in vec3 TexCoords;

out vec4 color;

uniform vec3 lightColor;
uniform float shineDamper;
uniform float reflectivity;
uniform samplerCube smile;
uniform float glow;

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

	vec3 I = normalize(toCamera);
	vec3 R = reflect(I, unitNormal);
	color = texture(smile, R);
	//color = vec4(1, 1, 0, 1);

	float edge = max(0, dot(unitNormal, toCamera));
	if( edge < 0.7f){
		if(glow != 0){
			color = vec4(0, 0, 1, edge);
		} else {
			color = vec4(0,0,0,0);
		}
	}
}
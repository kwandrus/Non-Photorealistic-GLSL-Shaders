// Toon Shading - Fragment Shader
#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 normalOut;
layout (location = 2) out vec4 depthOut;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 coolColor;
uniform vec3 warmColor;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float specularStrength = 0.5;
uniform float alpha = 0.0;
uniform float beta = 0.0;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	// if angle between norm and lightDir > 90 degrees, dot product is neg
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 finalCool = coolColor + alpha * objectColor;
	vec3 finalWarm = warmColor + beta * objectColor;

	float lerp = (1.0 + diff) / 2.0;

	finalCool = (1 - lerp) * finalCool;
	finalWarm = lerp * finalWarm;

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	FragColor = vec4(finalCool + finalWarm + specular, 1.0);
	normalOut = vec4(Normal, 1.0);

	depthOut = vec4(FragPos.z, FragPos.z, FragPos.z, 1.0);
}
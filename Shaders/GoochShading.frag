// Toon Shading - Fragment Shader
#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 normalOut;
layout (location = 2) out vec4 depthOut;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 coolColor;
uniform vec3 warmColor;
uniform vec3 objectColor;
uniform float alpha;
uniform float beta;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	// if angle between norm and lightDir > 90 degrees, dot product is neg
	float diff = max(dot(norm, lightDir), 0.0);

	vec3 finalCool = coolColor + alpha * objectColor;
	vec3 finalWarm = warmColor + beta * objectColor;

	float lerp = (1.0 + diff) / 2.0;

	finalCool = lerp * finalCool;
	finalWarm = (1 - lerp) * finalWarm;

	FragColor = vec4(finalCool + finalWarm, 1.0);
	normalOut = vec4(Normal, 0.0);
	depthOut = vec4(FragPos.z, 0.0, 0.0, 0.0);
}
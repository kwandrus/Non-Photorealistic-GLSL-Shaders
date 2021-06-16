// Toon Shading - Fragment Shader
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform int colorSteps = 3;
uniform vec3 objectColor;

void main()
{
	// ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	// if angle between normal and lightDir > 90 degrees, dot product is neg
	float diffuse = dot(normal, lightDir);
	float diffuseToon = max(ceil(diffuse * float(colorSteps)) / float(colorSteps), 0.0);

	vec3 toonColor = diffuseToon * lightColor * objectColor;

	FragColor = vec4(toonColor, 1.0);
}
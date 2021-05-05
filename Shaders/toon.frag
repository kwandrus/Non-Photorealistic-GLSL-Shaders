// Toon Shading - Fragment Shader
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);\

	// if angle between norm and lightDir > 90 degrees, dot product is neg
	float intensity = max(dot(norm, lightDir), 0.0);

	vec3 result;
	if (intensity > 0.95)
		result = vec3(1.0, 0.5, 0.5);
	else if (intensity > 0.5)
		result = vec3(0.6, 0.3, 0.3);
	else if (intensity > 0.25)
		result = vec3(0.4, 0.2, 0.2);
	else
		result = vec3(0.2, 0.1, 0.1);

	FragColor = vec4(result, 1.0);
}
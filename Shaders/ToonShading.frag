// Toon Shading - Fragment Shader
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// texture samplers
uniform sampler2D texture_diffuse1;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform int colorSteps = 3;
uniform bool texturesToggle;
// uniform vec3 objectColor;

void main()
{
	// ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	// if angle between norm and lightDir > 90 degrees, dot product is neg
	float diff = dot(norm, lightDir);
	float diffToon = max(ceil(diff * float(colorSteps)) / float(colorSteps), 0.0);

	vec3 diffuse = diffToon * lightColor;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	// vec3 result = ambient + diffuse + specular;
	// FragColor = vec4(result, 1.0) * texture(texture_diffuse1, TexCoords);

	if (texturesToggle)
	{
		FragColor = vec4(diffuse, 1.0) * texture(texture_diffuse1, TexCoords);
	}
	else
	{
		FragColor = vec4(diffuse, 1.0);
	}
}
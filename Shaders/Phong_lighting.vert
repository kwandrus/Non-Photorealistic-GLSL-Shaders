// Phong Shading - Vertex Shader
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

layout(std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);

	// convert FragPos to world space for lighting calculations in the frag shader
	FragPos = vec3(model * vec4(aPos, 1.0));

	Normal = normalMatrix * aNormal;

	TexCoords = aTexCoords;
}
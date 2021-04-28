#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
// uniform mat3 normalMatrix;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0f);

	// convert FragPos to world space for lighting calculations in the frag shader
	FragPos = vec3(model * vec4(aPos, 1.0));

	// multiply by normal matrix
	Normal = mat3(transpose(inverse(model))) * aNormal;
}
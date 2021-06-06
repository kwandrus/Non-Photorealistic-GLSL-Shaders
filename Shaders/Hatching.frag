// Hatching - Fragment Shader
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D hatching0; // light
uniform sampler2D hatching1;
uniform sampler2D hatching2;
uniform sampler2D hatching3;
uniform sampler2D hatching4;
uniform sampler2D hatching5; // dark

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	// if angle between norm and lightDir > 90 degrees, dot product is neg
	float diff = max(dot(norm, lightDir), 0.0);
	//diff = diff * 3(0.2326, 0.7152, 0.0722));

	// 6 texture lookups NOT great for performance -> change to Texture Array?
	float hatch0 = texelFetch( hatching0, ivec2(gl_FragCoord), 0 ).r;
	float hatch1 = texelFetch( hatching1, ivec2(gl_FragCoord), 0 ).r;
	float hatch2 = texelFetch( hatching2, ivec2(gl_FragCoord), 0 ).r;
	float hatch3 = texelFetch( hatching3, ivec2(gl_FragCoord), 0 ).r;
	float hatch4 = texelFetch( hatching4, ivec2(gl_FragCoord), 0 ).r;
	float hatch5 = texelFetch( hatching5, ivec2(gl_FragCoord), 0 ).r;

	/*float hatch0 = texture(hatching0, TexCoords).r;
	float hatch1 = texture(hatching1, TexCoords).r;
	float hatch2 = texture(hatching2, TexCoords).r;
	float hatch3 = texture(hatching3, TexCoords).r;
	float hatch4 = texture(hatching4, TexCoords).r;
	float hatch5 = texture(hatching5, TexCoords).r;*/

	// weight math arithmetic from http://kylehalladay.com/blog/tutorial/2017/02/21/Pencil-Sketch-Effect.html
	//float lightIntensity = dot(vec3(diff, diff, diff), vec3(0.2326, 0.7152, 0.0722)) * 6;
	//vec3 lightIntensity3 = vec3(lightIntensity, lightIntensity, lightIntensity);
	vec3 lightIntensity3 = vec3(diff, diff, diff) * 6;
	vec3 weights0 = clamp(lightIntensity3 - vec3(0, 1, 2), 0.0, 1.0);
	vec3 weights1 = clamp(lightIntensity3 - vec3(3, 4, 5), 0.0, 1.0);

	weights0.xy -= weights0.yz;
	weights0.z -= weights1.x;
	weights1.xy -= weights1.yz;

	vec3 hatchingColor = vec3(0.0, 0.0, 0.0);
	hatchingColor += hatch5 * weights0.x;
	hatchingColor += hatch4 * weights0.y;
	hatchingColor += hatch3 * weights0.z;
	hatchingColor += hatch2 * weights1.x;
	hatchingColor += hatch1 * weights1.y;
	hatchingColor += hatch0 * weights1.z;

	FragColor = vec4(hatchingColor, 1.0);
}
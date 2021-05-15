// Sobel Outline Filter - Fragment Shader
 #version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D imageTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;

uniform float _OutlineThickness;
uniform float _OutlineDepthMultiplier;
uniform float _OutlineDepthBias;
uniform float _OutlineNormalMultiplier;
uniform float _OutlineNormalBias;

uniform vec4 _OutlineColor;


float CalculateDepthGradient()
{
	mat3 I;

	// fetch the 3x3 neighbourhood
	for (int i=0; i<3; i++)
	for (int j=0; j<3; j++) {
		I[i][j] = texelFetch( depthTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).r;
	}

	float center = I[1][1];
	float gradient = (abs(I[0][0]) - center) + (abs(I[0][1]) - center) + (abs(I[0][2]) - center) + 
					 (abs(I[1][0]) - center)						   + (abs(I[1][2]) - center) +
					 (abs(I[2][0]) - center) + (abs(I[2][1]) - center) + (abs(I[2][2]) - center);

	return pow(clamp(gradient, 0.0, 1.0) * _OutlineDepthMultiplier, _OutlineDepthBias);
}

float CalculateNormalGradient()
{
	vec3 sample, gradient;
	gradient = vec3(0.0,0.0,0.0);
	vec3 center = texelFetch( normalTexture, ivec2(gl_FragCoord), 0 ).xyz;

	// fetch the 3x3 neighbourhood and use the RGB vector's length as intensity value
	for (int i=0; i<3; i++)
	for (int j=0; j<3; j++) {
		if (i != 1 && j != 1)
		{
			sample = texelFetch( normalTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).xyz;
			gradient += abs(sample - center);
		}
	}

	float sobelNormal = gradient.x + gradient.y + gradient.z;
	return pow(clamp(sobelNormal, 0.0, 1.0) * _OutlineNormalMultiplier, _OutlineNormalBias);
}

void main(void)
{
	//float3 offset = float3((1.0 / _ScreenParams.x), (1.0 / _ScreenParams.y), 0.0) * _OutlineThickness;
    vec3 sceneColor = texture(imageTexture, TexCoords).rgb;

	float sobelDepth = CalculateDepthGradient();
	float sobelNormal = CalculateNormalGradient();
	
	float sobelOutline = clamp(max(sobelDepth, sobelNormal), 0.0, 1.0);
	vec3 outlineColor = mix(sceneColor, _OutlineColor.rgb, _OutlineColor.a);
    vec3 color = mix(sceneColor, outlineColor, sobelOutline);

	FragColor = vec4(color, 1.0);
}
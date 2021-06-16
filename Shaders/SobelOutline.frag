// Sobel Outline Filter - Fragment Shader
 #version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D imageTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;

uniform float depthThreshold;
uniform float normalThreshold;

uniform vec4 _OutlineColor;


mat3 contourFilter = mat3( 
    1.0, 2.0, 1.0, 
    2.0, 0.0, 2.0, 
    1.0, 2.0, 1.0 
);


float CalculateDepthGradient() // calculates silhouette edges
{
	mat3 I;
	float sample;
	float center = texelFetch( depthTexture, ivec2(gl_FragCoord), 0 ).r;

	// fetch the 3x3 neighbourhood
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++) 
		{
			sample = texelFetch( depthTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).r;

			// apply contour detection filter
			I[i][j] = abs(sample - center) * contourFilter[i][j] / 8.0;
		}
	}

	float gmax = I[1][1], gmin = I[1][1];
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++) 
		{
			if (gmax < I[i][j])
				gmax = I[i][j];
			if (gmin > I[i][j])
				gmin = I[i][j];
		}
	}

	return min(pow((gmax - gmin) / depthThreshold, 2), 1);
}

float CalculateNormalGradient() // calculates crease edges
{
	vec3 sample;
	mat3 I;
	vec3 center = texelFetch( normalTexture, ivec2(gl_FragCoord), 0 ).xyz;

	// fetch the 3x3 neighbourhood and use the RGB vector's length as intensity value
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++) 
		{
			sample = texelFetch( normalTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).xyz;
			I[i][j] = length(sample - center) * contourFilter[i][j] / 8.0;
		}
	}

	float gmax = I[1][1], gmin = I[1][1];
	for (int i=0; i<3; i++)
	{
		for (int j=0; j<3; j++) 
		{
			if (gmax < I[i][j])
				gmax = I[i][j];
			if (gmin > I[i][j])
				gmin = I[i][j];
		}
	}

	return min(pow((gmax - gmin) / normalThreshold, 2), 1);
}

void main(void)
{
    vec3 sceneColor = texture(imageTexture, TexCoords).rgb;

	float sobelDepth = CalculateDepthGradient();
	float sobelNormal = CalculateNormalGradient();
	
	float sobelOutline = clamp(max(sobelDepth, sobelNormal), 0.0, 1.0);
	vec3 outlineColor = mix(sceneColor, _OutlineColor.rgb, _OutlineColor.a);
    vec3 color = mix(sceneColor, outlineColor, sobelOutline);

	FragColor = vec4(color, 1.0);
}
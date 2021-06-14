// Sobel Outline Filter - Fragment Shader
 #version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D imageTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;

//uniform float _OutlineThickness;
//uniform float _OutlineDepthMultiplier;
//uniform float _OutlineDepthBias;
//uniform float _OutlineNormalMultiplier;
//uniform float _OutlineNormalBias;
uniform float depthThreshold;
uniform float normalThreshold;

uniform vec4 _OutlineColor;


mat3 sobelKernel_x = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);
mat3 sobelKernel_y = mat3( // transpose of the x direction
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);
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

	// normalize
	//I = I / 8;

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
	

	//float center = I[1][1];
	//float gradient = (abs(I[0][0]) - center) + (abs(I[0][1]) - center) + (abs(I[0][2]) - center) + 
	//				 (abs(I[1][0]) - center)						   + (abs(I[1][2]) - center) +
	//				 (abs(I[2][0]) - center) + (abs(I[2][1]) - center) + (abs(I[2][2]) - center);

	//return pow(clamp(gradient, 0.0, 1.0) * _OutlineDepthMultiplier, _OutlineDepthBias);

	//float gx = dot(sobelKernel_x[0], I[0]) + dot(sobelKernel_x[1], I[1]) + dot(sobelKernel_x[2], I[2]); 
	//float gy = dot(sobelKernel_y[0], I[0]) + dot(sobelKernel_y[1], I[1]) + dot(sobelKernel_y[2], I[2]);

	//float g = sqrt(pow(gx, 2.0) + pow(gy, 2.0));
	//return g;
	//color = vec4(diffuse - vec3(g), 1.0);
}

float CalculateNormalGradient() // calculates crease edges
{
	vec3 sample;
	//vec3 gradient[9];
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

	// normalize
	//I = I / 8;

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

	//float sobelNormal = gradient.x + gradient.y + gradient.z;
	//return pow(clamp(sobelNormal, 0.0, 1.0) * _OutlineNormalMultiplier, _OutlineNormalBias);
}

void main(void)
{
	//float3 offset = float3((1.0 / _ScreenParams.x), (1.0 / _ScreenParams.y), 0.0) * _OutlineThickness;
    vec3 sceneColor = texture(imageTexture, TexCoords).rgb;
	//vec3 sceneColor = vec3(1,1,1);

	float sobelDepth = CalculateDepthGradient();
	float sobelNormal = CalculateNormalGradient();
	//float sobelDepth = 0;
	
	float sobelOutline = clamp(max(sobelDepth, sobelNormal), 0.0, 1.0);
	//float sobelOutline = clamp(sobelNormal, 0.0, 1.0);
	vec3 outlineColor = mix(sceneColor, _OutlineColor.rgb, _OutlineColor.a);
    vec3 color = mix(sceneColor, outlineColor, sobelOutline);

	FragColor = vec4(color, 1.0);
	//FragColor = vec4(sceneColor, 1.0);
}
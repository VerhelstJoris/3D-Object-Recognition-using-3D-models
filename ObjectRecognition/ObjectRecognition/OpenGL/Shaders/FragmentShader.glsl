#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragmentColor;

// Ouput data
layout(location = 0) out vec3 color;

// Ouput data
//out vec3 color;

void main()
{
	//FOR THE PURPOSED OF THE SILHOUETTE EXTRACTION
	//SIMPLE RETURN COLOR IS IDEAL, NO SHADING OR COLOR VARIATION
	color = vec3(0,1,0);

}
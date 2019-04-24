#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos; // Distance from local origin

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(fcolor * vcolor, 1.0);

	// Salmon mesh is contained in a 2x2 square (see salmon.mesh for vertices)
	float radius = distance(vec2(0.0), vpos);
}
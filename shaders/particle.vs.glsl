#version 330 
// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;
out vec4 vcolor;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform mat3 view;
uniform float time;
uniform float life_time;

void main()
{
	vcolor = vec4(1.f, 1.f, 1.f, (life_time-time)/life_time);
	texcoord = in_texcoord;
	vec3 pos = projection * view * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
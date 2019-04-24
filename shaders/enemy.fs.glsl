#version 330
// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform float distort;

// Output color
layout(location = 0) out  vec4 color;

vec2 dis(vec2 uv){
	vec2 coord = uv.xy;
	coord.x = 0.98*coord.x + 0.02*sin(5*coord.x + distort/100);
	coord.y = 0.99*coord.y + 0.01*cos(5*coord.y + distort/100);
	if(distort == 0.f) return uv;
	return coord;
}

void main()
{
	vec2 coord = texcoord;
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(coord.x, coord.y));
	if(distort != 0.f){
		color.g += 0.5*abs(sin(distort/100));
	}
}

#version 330


in vec3 in_position;
in vec2 in_texcoord;

out vec4 fragColor;
uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

uniform mat3 transform;
uniform mat3 projection;
uniform mat3 view;

float snow(vec2 uv,float scale)
{
    float w=smoothstep(1.,0.,-uv.y*(scale/10.));if(w<.1)return 0.;
    uv+=time/scale;uv.y+=time*2./scale;uv.x+=sin(uv.y+time*.5)/scale;
    uv*=scale;vec2 s=floor(uv),f=fract(uv),p;float k=3.,d;
    p=.5+.35*sin(11.*fract(sin((s+p+scale)*mat2(7,3,6,5))*5.))-f;d=length(p);k=min(d,k);
    k=smoothstep(0.,k,sin(f.x+f.y)*0.01);
    return k*w;
}

void main(){
    vec2 uv=(fragColor.xy*2.-resolution.xy)/min(resolution.x,resolution.y);
    vec3 finalColor=vec3(0);
    float c=smoothstep(1.,0.3,clamp(uv.y*.3+.8,0.,.75));
    c+=snow(uv,30.)*.3;
    c+=snow(uv,20.)*.5;
    c+=snow(uv,15.)*.8;
    c+=snow(uv,10.);
    c+=snow(uv,8.);
    c+=snow(uv,6.);
    c+=snow(uv,5.);
    finalColor=(vec3(c));
    fragColor = vec4(finalColor,1);
    
    vec3 pos = projection * view * transform * vec3(in_position.xy, 1.0);
    gl_Position = vec4(pos.xy, in_position.z, 1.0);
}

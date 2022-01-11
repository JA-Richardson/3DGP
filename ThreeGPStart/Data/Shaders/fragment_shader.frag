#version 330

uniform sampler2D sampler_tex;

uniform vec3 light_intensity;

uniform vec4 diffuse_colour;
in vec3 varying_colour;
in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_texcoord;

in vec3 varying_gouraud;


out vec4 fragment_colour;


void main(void)
{ 
	vec3 tex_colour = texture(sampler_tex, varying_texcoord).rgb;
	vec3 N = normalize(varying_normal);
	vec3 P = varying_position;
	vec3 light_position = vec3(50, 100, 50);
	vec3 light_direction = vec3(-0.5, -0.1, 0);
	vec3 point_light_direction = P - light_position;
	vec3 L = normalize(-light_direction);
	vec3 LP = normalize(-point_light_direction);
	float light_intensity = max(0,dot(L,N));
	float point_light_intensity = max(0,dot(LP,N));
	tex_colour = (light_intensity + point_light_intensity) *  tex_colour;
	fragment_colour=vec4(tex_colour,1.0);
}
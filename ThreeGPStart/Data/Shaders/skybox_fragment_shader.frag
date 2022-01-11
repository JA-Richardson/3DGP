#version 330

//uniform sampler2D sampler_tex;

uniform samplerCube skybox;

uniform vec4 diffuse_colour;

in vec3 varying_coords;

out vec4 fragment_colour;

void main(void)
{
	fragment_colour = texture(skybox, varying_coords);
	
}
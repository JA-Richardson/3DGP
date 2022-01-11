#version 330

uniform mat4 combined_xform_sky;
uniform mat4 model_xform;

layout (location=0) in vec3 vertex_position;
//layout (location=1) in vec3 vertex_colour;

out vec3 varying_coords;

void main(void)
{	
	varying_coords = vertex_position * vec3(1, -1, 1);

	gl_Position = combined_xform_sky * model_xform * vec4(vertex_position, 1.0);
}
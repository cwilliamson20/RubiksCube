#version 400
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 pos;

// input color data
layout (location = 1) in vec3 color;

// output data that the fragment shader uses
out vec3 fragment_color;

// take in uniforms
uniform mat4 transform;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  transform * vec4(pos, 1);
    fragment_color = color;
}
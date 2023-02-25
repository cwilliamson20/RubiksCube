#version 400
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// input color data, the second buffer (location = 1)
layout(location = 1) in vec3 vertexColor;

// output data that the fragment shader uses
out vec3 fragment_color;
// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
    fragment_color = vertexColor;
}
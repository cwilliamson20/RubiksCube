#version 400
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 aPos;

// output data that the fragment shader uses
out vec3 fragment_color;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  vec4(aPos,1);
    fragment_color = vec3(0, 1, 1);
}
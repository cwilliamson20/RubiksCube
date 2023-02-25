#version 400
layout(location = 0) in vec3 vertexPosition_modelspace;
void main() {
    // set the vertex position to whatever was in the buffer
    gl_Position.xyz = vertexPosition_modelspace;
    gl_Position.w = 1.0;
}
#version 400
in vec3 fragment_color;
out vec3 color;
void main() {
    // the color of each vertex is interpolated across the triangle
    color = fragment_color;
}
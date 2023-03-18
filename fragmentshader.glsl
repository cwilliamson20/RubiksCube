#version 400
in vec3 fragment_color;
in vec2 texture_coord;

out vec4 frag_color;
uniform sampler2D ourTexture;
void main() {
    // the color of each vertex is interpolated across the triangle
    frag_color = texture(ourTexture, texture_coord) * vec4(fragment_color, 1.0);
}
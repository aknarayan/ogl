#version 330 core

// interpolated values from vertex shaders
in vec3 fragmentColour;

// output data
out vec3 colour;

void main() {
    // output colour = colour specified in vertex shader, interpolated between all 3 surrounding vertices
    colour = fragmentColour;
}
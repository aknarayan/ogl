#version 330 core

// input vertex data, different for all executions of this shader
layout(location = 0) in vec3 vertexPosition_modelspace;

// input colour data - notice that the "1" equals the glEnableVertexAttribArray
layout(location = 1) in vec3 vertexColour;

// values that stay constant for the whole mesh
uniform mat4 MVP;

// output data (forwarded to fragment shader), will be interpolated for each fragment
out vec3 fragmentColour;

void main() {
    // output position of vertex in clip space = mvp * position
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

    fragmentColour = vertexColour;
}

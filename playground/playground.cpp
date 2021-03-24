// standard headers
#include <stdio.h>
#include <stdlib.h>

// always include glew.h before gl.h and glfw3.h
#include <GL/glew.h>

// GLFW handles window and keyboard
#include <GLFW/glfw3.h>

// provides functionality for loading shaders
#include <common/shader.hpp>

// maths library
#include <glm/glm.hpp>

// matrix transform functions
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

int main(void)
{
	// initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL 3.3
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // to make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // we don't want old OpenGL

	// open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow(1024, 768, "Playground", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// initialize GLEW
	glewExperimental = true; // needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// VertexArrayObject, VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// create and compile GLSL program from shaders
	GLuint programID = LoadShaders("simplevertexshader.glsl", "simplefragmentshader.glsl");

	// get handle for MVP uniform, only done during initialisation
	GLuint matrixID = glGetUniformLocation(programID, "MVP");

	// perspective projection matrix: 45° fov, 4:3 aspect ratio, near clipping plane 0.1, far clipping plane 100.0
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
	
	// orthographic projection matrix
	//glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);

	// camera (view) matrix
	glm::mat4 view = glm::lookAt(
		glm::vec3(4, 3, -3), // camera is at (4, 3, -3) in world space
		glm::vec3(0, 0, 0),  // camera looks at origin
		glm::vec3(0, 1, 0)   // up vector is (0, 1, 0)
	);

	// model matrix (identity, i.e. model will be at origin)
	glm::mat4 model = glm::mat4(1.0f);

	// model-view-projection matrix
	glm::mat4 mvp = projection * view * model;

	// array of 36 vectors, each representing a triangle vertex
	// 3 consecutive vertices represent a triangle
	// 2 consecutive triangles represent a cube face
	static const GLfloat g_vertex_buffer_data[] = {
	-1.0f,-1.0f,-1.0f, // triangle 1 begin
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 end
	1.0f, 1.0f, -1.0f, // triangle 2 begin
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, // triangle 2 end
	1.0f, -1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f,-1.0f, -1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f,-1.0f, -1.0f,
	 1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f,-1.0f,
	 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
	};

	GLuint vertexBuffer; // vertex buffer ID
	glGenBuffers(1, &vertexBuffer); // generate 1 buffer, put resulting identifier in vertexBuffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW); // give vertices to OpenGL

	// one randomnly generated colour for each vertex
	static const GLfloat g_colour_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};

	GLuint colourBuffer; // colour buffer ID
	glGenBuffers(1, &colourBuffer); // generate 1 buffer, put resulting identifier in colourBuffer
	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_colour_buffer_data), g_colour_buffer_data, GL_STATIC_DRAW); // give colours to OpenGL

	do {
		// clear the screen - can cause flickering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use shader
		glUseProgram(programID);

		// enable depth test
		glEnable(GL_DEPTH_TEST);
		
		// accept fragment if it closer to the camera than the former one (i.e. z-buffering)
		glDepthFunc(GL_LESS);

		// send transformation to currently bound shader, in the "MVP" uniform
		// done in the loop as each model will have a different M matrix and therefore a different MVP matrix
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

		// 1st attribute buffer: vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(
			0,			// attribute 0 - no particular reason, but must match layout in shader
			3,			// size
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,			// stride
			(void*)0	// array buffer offset
		);

		// 2nd attribute buffer: colours
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
		glVertexAttribPointer(
			1,          // attribute 1 - no particular reason, but must match layout in shader
			3,          // size
			GL_FLOAT,   // type
			GL_FALSE,   // normalized?
			0,          // stride
			(void*)0    // array buffer offset
		);

		// draw triangle
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // starting from vertex 0; 12 * 3 vertices total -> 12 triangles -> 6 square faces
		glDisableVertexAttribArray(0);

		// swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} // check if ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0);

	// cleanup VBO and shader
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


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
		glm::vec3(4, 3, 3), // camera is at (4, 3, 3) in world space
		glm::vec3(0, 0, 0), // camera looks at origin
		glm::vec3(0, 1, 0)  // up vector is (0, 1, 0)
	);

	// model matrix (identity, i.e. model will be at origin)
	glm::mat4 model = glm::mat4(1.0f);

	// model-view-projection matrix
	glm::mat4 mvp = projection * view * model;

	// array of 3 vectors, representing 3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	GLuint vertexBuffer; // vertex buffer ID
	glGenBuffers(1, &vertexBuffer); // generate 1 buffer, put resulting identifier in vertexBuffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW); // give vertices to OpenGL

	do {
		// clear the screen - can cause flickering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use shader
		glUseProgram(programID);

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

		// draw triangle
		glDrawArrays(GL_TRIANGLES, 0, 3); // starting from vertex 0; 3 vertices total -> 1 triangle
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


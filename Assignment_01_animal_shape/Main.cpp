#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Geo_vertices.h"
#include <vector>

using namespace std;

int WIN_WIDTH = 1000;
int WIN_HEIGHT = 600;
char WIN_TITLE[] = "Assignment-1: Rhino";

void frameBbufferSizeCallback(GLFWwindow* window, int width, int height);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
	if (window == NULL) {
		cout << "FAILED TO CREATE WINDOW\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frameBbufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "FILED TO LOAD GLAD\n";
		glfwTerminate();
		return -1;
	}

	
	// shader

	const char* vsSrc = "#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"out vec3 color;\n"
		"uniform vec3 myColor;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(aPos, 0.0f, 1.0f);\n"
		"color = myColor;\n "
		"}\0";

	const char* fsSrc = "#version 330 core\n"
		"out vec4 FragColor;\n"	
		"in vec3 color;\n"
		"void main()\n"
		"{\n"
		"FragColor = vec4(color, 1.0f);\n"
		"}\0";

	unsigned int vs, fs, sp;
	int success;
	char infolog[512];

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsSrc, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vs, 512, NULL, infolog);
		cout << "ERROR IN COMPILING VERTEX SHADER\n" << infolog << "\n";
	}

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsSrc, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fs, 512, NULL, infolog);
		cout << "ERROR IN COMPILING FRAGMENT SHADER\n" << infolog << "\n";
	}

	sp = glCreateProgram();
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glLinkProgram(sp);
	glGetProgramiv(sp, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(sp, 512, NULL, infolog);
		cout << "ERROR IN LINKING SHADER PROGRAM\n" << infolog << "\n";
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	
	// buffer

	Geo_vertices vertices;
	int num_buffers = vertices.shapes.size();
	//cout << num_buffers << "\n";

	unsigned int VAO[8], VBO[8];

	glGenVertexArrays(num_buffers, VAO);
	glGenBuffers(num_buffers, VBO);

	for (int i = 0; i < num_buffers; i++) {
		glBindVertexArray(VAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]); 
		glBufferData(GL_ARRAY_BUFFER, vertices.shapes[i].size() * sizeof(float), &vertices.shapes[i][0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);  
		glEnableVertexAttribArray(0); 		
		glBindBuffer(GL_ARRAY_BUFFER, 0);  

		glBindVertexArray(0); 
	}

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



	 // render

	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(sp);

		for (int i = 0; i < num_buffers; i++) {
			glBindVertexArray(VAO[i]);
			if (i == 0) {
				glUniform3f(glGetUniformLocation(sp, "myColor"), vertices.colors[0][0], vertices.colors[0][1], vertices.colors[0][2]);
				glDrawArrays(GL_TRIANGLES, 0, vertices.shapes[i].size() / 2);
			}
			else if (i == 1) {
				glUniform3f(glGetUniformLocation(sp, "myColor"), vertices.colors[1][0], vertices.colors[1][1], vertices.colors[1][2]);
				glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.shapes[i].size() / 2);
			}
			else if (i == 2) {
				glUniform3f(glGetUniformLocation(sp, "myColor"), vertices.colors[2][0], vertices.colors[2][1], vertices.colors[2][2]);
				glDrawArrays(GL_LINE_STRIP, 0, vertices.shapes[i].size() / 2);
			}
			else if (i == 3) {
				glUniform3f(glGetUniformLocation(sp, "myColor"), vertices.colors[2][0], vertices.colors[2][1], vertices.colors[2][2]);
				glDrawArrays(GL_LINE_LOOP, 0, vertices.shapes[i].size() / 2); 
			}
			else {
				glUniform3f(glGetUniformLocation(sp, "myColor"), vertices.colors[3][0], vertices.colors[3][1], vertices.colors[3][2]);
				glDrawArrays(GL_LINE_STRIP, 0, vertices.shapes[i].size() / 2);  
			}
			glBindVertexArray(0);
		}			

		glfwSwapBuffers(window); 
		glfwPollEvents(); 
	}

	glDeleteVertexArrays(num_buffers, VAO); 
	glDeleteBuffers(num_buffers, VBO); 
	glDeleteProgram(sp);

	glfwTerminate();
	return 0;

}

void frameBbufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

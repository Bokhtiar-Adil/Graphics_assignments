#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

using namespace std;

Camera camera(glm::vec3(0.4f, 0.4f, 2.8f));
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

int WIN_WIDTH = 1000;
int WIN_HEIGHT = 800;
float TABLE_BASE_WIDTH = 0.6f;
float TABLE_LEG_WIDTH = (float) (TABLE_BASE_WIDTH * 0.067);


void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1000, 600, "Textures", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to load window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

	// glad loading

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to load GLAD\n";
		return -1;
	}

	// shader creation

	Shader shader("vsSrc.vs", "fsSrc.fs");
	
	// vertices and buffers

	float vertices[] = {
		0.0f, 0.0f, 0.0f,  // 0
		0.6f, 0.0f, 0.0f,  // 1
		0.6f, 0.0f, -0.6f, // 2
		0.0f, 0.0f, -0.6f,  // 3
		0.0f, 0.1f, 0.0f,  // 4
		0.6f, 0.1f, 0.0f,  //5
		0.6f, 0.1f, -0.6f,  // 6
		0.0f, 0.1f, -0.6f,  // 7
	};

	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,
		4, 5, 7,
		5, 6, 7,
		0, 1, 5,
		0, 5, 4,
		1, 2, 6,
		1, 6, 5,
		3, 2, 6,
		3, 6, 7,
		0, 3, 7,
		0, 7, 4,
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glEnable(GL_DEPTH_TEST);

	// render

	glm::mat4 identityMatrix = glm::mat4(1.0f);
	glm::mat4 translate, rotate, scale, tableRowGap, tableColumnGap, legGapSepTable;

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		processInput(window);
	
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		shader.use();
		glBindVertexArray(VAO);
		
		// tables
		for (int i = 0; i < 4; i++) {
			tableRowGap = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, i*1.2f));
			for (int j = 0; j < 4; j++) {
				tableColumnGap = glm::translate(tableRowGap, glm::vec3(j * 0.8f, 0.0f, 0.0f));
				glm::mat4 model = glm::mat4(1.0f); 				
				//glm::mat4 view = glm::mat4(1.0f);
				glm::mat4 view = camera.GetViewMatrix();
				glm::mat4 projection = glm::mat4(1.0f);
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.5f, 1.0f, 0.0f));
				model = tableColumnGap * model;
				view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
				projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
				shader.setMat4("view", view);
				shader.setMat4("projection", projection);
				shader.setMat4("model", model);
				shader.setVec3("color", 1.0f, 0.0f, 0.0f);
				//glDrawArrays(GL_TRIANGLES, 0, 36);				
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // base

				shader.setVec3("color", 0.78f, 0.62f, 0.01f);
				rotate = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				scale = glm::scale(identityMatrix, glm::vec3(0.067f, 1.0f, 0.067f));
				translate = glm::translate(identityMatrix, glm::vec3(0.0f, i*-1.2f, 0.0f));
				model = translate * scale * rotate * model;
				// scaling makes j*0.8 -> j*0.8*0.067 -> translate shifts it to pos j*0.8*0.067 + 0.8 -> this excess shift needs to be reversed
				legGapSepTable = glm::translate(identityMatrix, glm::vec3(-0.8*j*0.067, 0.0f, 0.0f)); 				
				model = legGapSepTable * tableColumnGap * model;
				shader.setMat4("model", model);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // front left leg

				translate = glm::translate(identityMatrix, glm::vec3(TABLE_BASE_WIDTH - TABLE_LEG_WIDTH, 0, 0));
				model = translate * model;
				shader.setMat4("model", model);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // front right leg

				translate = glm::translate(identityMatrix, glm::vec3(0, 0, -1 * (TABLE_BASE_WIDTH - TABLE_LEG_WIDTH)));
				translate = glm::translate(translate, glm::vec3(-1 * (TABLE_BASE_WIDTH - TABLE_LEG_WIDTH), 0, 0));
				model = translate * model;
				shader.setMat4("model", model);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // back left leg

				translate = glm::translate(identityMatrix, glm::vec3(TABLE_BASE_WIDTH - TABLE_LEG_WIDTH, 0, 0));
				model = translate * model;
				shader.setMat4("model", model);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // back right leg
			}
		}

		// chairs


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	shader.deleteProgram();

	glfwTerminate();
	return 0;
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}




//float vertices[] = {
//		-0.5f, -0.5f, -0.5f,
//		 0.5f, -0.5f, -0.5f,
//		 0.5f,  0.5f, -0.5f,
//		 0.5f,  0.5f, -0.5f,
//		-0.5f,  0.5f, -0.5f,
//		-0.5f, -0.5f, -0.5f,
//
//		-0.5f, -0.5f,  0.5f,
//		 0.5f, -0.5f,  0.5f,
//		 0.5f,  0.5f,  0.5f,
//		 0.5f,  0.5f,  0.5f,
//		-0.5f,  0.5f,  0.5f,
//		-0.5f, -0.5f,  0.5f,
//
//		-0.5f,  0.5f,  0.5f,
//		-0.5f,  0.5f, -0.5f,
//		-0.5f, -0.5f, -0.5f,
//		-0.5f, -0.5f, -0.5f,
//		-0.5f, -0.5f,  0.5f,
//		-0.5f,  0.5f,  0.5f,
//
//		 0.5f,  0.5f,  0.5f,
//		 0.5f,  0.5f, -0.5f,
//		 0.5f, -0.5f, -0.5f,
//		 0.5f, -0.5f, -0.5f,
//		 0.5f, -0.5f,  0.5f,
//		 0.5f,  0.5f,  0.5f,
//
//		-0.5f, -0.5f, -0.5f,
//		 0.5f, -0.5f, -0.5f,
//		 0.5f, -0.5f,  0.5f,
//		 0.5f, -0.5f,  0.5f,
//		-0.5f, -0.5f,  0.5f,
//		-0.5f, -0.5f, -0.5f,
//
//		-0.5f,  0.5f, -0.5f,
//		 0.5f,  0.5f, -0.5f,
//		 0.5f,  0.5f,  0.5f,
//		 0.5f,  0.5f,  0.5f,
//		-0.5f,  0.5f,  0.5f,
//		-0.5f,  0.5f, -0.5f,
//};

//float vertices[] = {
//		-0.5f, -0.5f, -0.5f,  
//		 0.5f, -0.5f, -0.5f,  
//		 0.5f,  0.5f, -0.5f,  
//		 0.5f,  0.5f, -0.5f,  
//		-0.5f,  0.5f, -0.5f,  
//		-0.5f, -0.5f, -0.5f,  
//
//		-0.5f, -0.5f,  0.5f,  
//		 0.5f, -0.5f,  0.5f,  
//		 0.5f,  0.5f,  0.5f, 
//		 0.5f,  0.5f,  0.5f, 
//		-0.5f,  0.5f,  0.5f,  
//		-0.5f, -0.5f,  0.5f,  
//
//		-0.5f,  0.5f,  0.5f,  
//		-0.5f,  0.5f, -0.5f,  
//		-0.5f, -0.5f, -0.5f,  
//		-0.5f, -0.5f, -0.5f,  
//		-0.5f, -0.5f,  0.5f,  
//		-0.5f,  0.5f,  0.5f,  
//
//		 0.5f,  0.5f,  0.5f,  
//		 0.5f,  0.5f, -0.5f,  
//		 0.5f, -0.5f, -0.5f,  
//		 0.5f, -0.5f, -0.5f,  
//		 0.5f, -0.5f,  0.5f,  
//		 0.5f,  0.5f,  0.5f,  
//
//		-0.5f, -0.5f, -0.5f,  
//		 0.5f, -0.5f, -0.5f,  
//		 0.5f, -0.5f,  0.5f,  
//		 0.5f, -0.5f,  0.5f,  
//		-0.5f, -0.5f,  0.5f, 
//		-0.5f, -0.5f, -0.5f,  
//
//		-0.5f,  0.5f, -0.5f,  
//		 0.5f,  0.5f, -0.5f, 
//		 0.5f,  0.5f,  0.5f,  
//		 0.5f,  0.5f,  0.5f, 
//		-0.5f,  0.5f,  0.5f, 
//		-0.5f,  0.5f, -0.5f,  
//};;
//unsigned int indices[] = {
//		0, 1, 3, // first triangle
//		1, 2, 3  // second triangle
//	};
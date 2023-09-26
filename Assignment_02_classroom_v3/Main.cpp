#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

using namespace std;

Camera camera(glm::vec3(2.3f, 0.95f, 4.0f));
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

bool firstMouse = true;
float yaw = -90.0f;	
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

int WIN_WIDTH = 1200;
int WIN_HEIGHT = 800;
float TABLE_BASE_WIDTH = 0.6f;
float TABLE_LEG_WIDTH = (float) (TABLE_BASE_WIDTH * 0.067);
float TABLE_COLUMN_GAP = 1.0f;
float TABLE_ROW_GAP = 1.2f;
float FLOOR_TILES_WIDTH = TABLE_BASE_WIDTH * 1.2f;
float FLOOR_TILES_GAP = 0.005f;

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Assignment-02: Classroom", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to load window\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

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
	glm::mat4 translate, rotate, scale, tableRowGap, tableColumnGap, legGapSepTable, gap, chair, locker, teacher, fan, flr, walls, ceiling, tmpflr, board;

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
			tableRowGap = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, i*TABLE_ROW_GAP));
			for (int j = 0; j < 4; j++) {
				tableColumnGap = glm::translate(tableRowGap, glm::vec3(j * TABLE_COLUMN_GAP, 0.0f, 0.0f));
				glm::mat4 model = glm::mat4(1.0f); 											
				glm::mat4 view = camera.GetViewMatrix();
				glm::mat4 projection = glm::mat4(1.0f);
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.5f, 1.0f, 0.0f));
				model = tableColumnGap * model;				
				projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
				shader.setMat4("view", view);
				shader.setMat4("projection", projection);
				shader.setMat4("model", model);
				shader.setVec3("color", 1.0f, 0.0f, 0.0f);							
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // base

				chair = model;
				if (i == 0) {
					locker = model;
					teacher = model;
				}
				if (i == 0 && j == 0) flr = model;
				if (i == 0 && j == 1) fan = model;

				shader.setVec3("color", 0.78f, 0.62f, 0.01f);
				rotate = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				scale = glm::scale(identityMatrix, glm::vec3(0.067f, 1.0f, 0.067f));
				translate = glm::translate(identityMatrix, glm::vec3(0.0f, i*-TABLE_ROW_GAP, 0.0f));
				model = translate * scale * rotate * model;
				// scaling makes j*TABLE_COLUMN_GAP -> j*TABLE_COLUMN_GAP*0.067 -> translate shifts it to pos j*TABLE_COLUMN_GAP*0.067 + TABLE_COLUMN_GAP -> this excess shift needs to be reversed
				legGapSepTable = glm::translate(identityMatrix, glm::vec3(-TABLE_COLUMN_GAP*j*0.067, 0.0f, 0.0f)); 				
				model = legGapSepTable * tableColumnGap * model;
				shader.setMat4("model", model);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // back left leg

				translate = glm::translate(identityMatrix, glm::vec3(TABLE_BASE_WIDTH - TABLE_LEG_WIDTH, 0, 0));
				model = translate * model;
				shader.setMat4("model", model);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // back right leg

				translate = glm::translate(identityMatrix, glm::vec3(0, 0, -1 * (TABLE_BASE_WIDTH - TABLE_LEG_WIDTH)));
				translate = glm::translate(translate, glm::vec3(-1 * (TABLE_BASE_WIDTH - TABLE_LEG_WIDTH), 0, 0));
				model = translate * model;
				shader.setMat4("model", model);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // front left leg

				translate = glm::translate(identityMatrix, glm::vec3(TABLE_BASE_WIDTH - TABLE_LEG_WIDTH, 0, 0));
				model = translate * model;
				shader.setMat4("model", model);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // front right leg


				// chairs				

				rotate = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));						
				scale = glm::scale(identityMatrix, glm::vec3(0.6f, 0.6f, 0.267f));				
				translate = glm::translate(identityMatrix, glm::vec3(0.0f, i*TABLE_ROW_GAP*0.6f, 0.1f));					
				chair = translate * scale * rotate * chair;				
				gap = glm::translate(identityMatrix, glm::vec3(j*TABLE_COLUMN_GAP*0.4+TABLE_BASE_WIDTH*0.2, 0.0f, i*TABLE_ROW_GAP));
				chair = gap * chair;
				shader.setVec3("color", 1.0f, 1.0f, 0.0f);
				shader.setMat4("model", chair);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // chair back 

				shader.setVec3("color", 0.4f, 0.29f, 0.004f);
				translate = glm::translate(identityMatrix, glm::vec3(0.0f, -0.3f, 0.0f));
				rotate = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));				
				chair = rotate * translate * chair;
				translate = glm::translate(identityMatrix, glm::vec3(0.0f, i * TABLE_ROW_GAP, i * TABLE_ROW_GAP+0.1));
				chair = translate * chair;
				shader.setMat4("model", chair);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // chair seat 

				
				rotate = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				scale = glm::scale(identityMatrix, glm::vec3(0.067f, 1.0f, 0.367f));
				translate = glm::translate(identityMatrix, glm::vec3(0.0f, -1 * (i * TABLE_ROW_GAP) - 0.1, 0.1f));
				chair = translate * scale * rotate * chair;
				// scaling makes j*TABLE_COLUMN_GAP -> j*TABLE_COLUMN_GAP*0.067 -> translate shifts it to pos j*TABLE_COLUMN_GAP*0.067 + TABLE_COLUMN_GAP -> this excess shift needs to be reversed
				legGapSepTable = glm::translate(identityMatrix, glm::vec3(-TABLE_COLUMN_GAP * j * 0.067, 0.0f, 0.0f));
				chair = legGapSepTable * tableColumnGap * chair;
				translate = glm::translate(identityMatrix, glm::vec3(TABLE_BASE_WIDTH*0.2f, -0.16f, 0.0f));
				chair = translate * chair;
				scale = glm::scale(identityMatrix, glm::vec3(1.0f, 1.28f, 1.0f));
				chair = scale * chair;
				shader.setMat4("model", chair);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // back left leg
				glm::mat4 fr = chair;

				translate = glm::translate(identityMatrix, glm::vec3(TABLE_BASE_WIDTH*0.6 - TABLE_LEG_WIDTH, 0, 0));
				chair = translate * chair;
				shader.setMat4("model", chair);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // back right leg
				
				translate = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -1 * ( TABLE_BASE_WIDTH * 0.6 - TABLE_LEG_WIDTH)));
				chair = translate * fr;
				shader.setMat4("model", chair);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // front left leg

				translate = glm::translate(identityMatrix, glm::vec3(TABLE_BASE_WIDTH*0.6 - TABLE_LEG_WIDTH, 0, 0));
				chair = translate * chair;
				shader.setMat4("model", chair);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // front right legchair

				shader.setVec3("color", 0.2f, 0.18f, 0.02f);
				scale = glm::scale(identityMatrix, glm::vec3(1.0f, 0.2f, 1.0f));
				translate = glm::translate(identityMatrix, glm::vec3(TABLE_BASE_WIDTH*0.2, 0.03f, -0.025f));
				chair = translate * scale * fr;
				shader.setMat4("model", chair);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // back-seat divider left

				translate = glm::translate(identityMatrix, glm::vec3(TABLE_BASE_WIDTH * 0.15, 0.0f, 0.0f));
				chair = translate * chair;
				shader.setMat4("model", chair);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // back-seat divider right

			}			
		}
		
		// teacher's table-chair
		translate = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -1.0f * TABLE_ROW_GAP *1.5f));
		teacher = translate * teacher;
		shader.setVec3("color", 1.0f, 0.0f, 0.0f);
		shader.setMat4("model", teacher);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // teacher's table base

		shader.setVec3("color", 0.78f, 0.62f, 0.01f); 
		rotate = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
		scale = glm::scale(identityMatrix, glm::vec3(0.067f, 1.0f, 0.067f)); 
		translate = glm::translate(identityMatrix, glm::vec3(-0.12f, -1.2f, 0.0f));  
		teacher = translate * scale * rotate * teacher;		 
		tableRowGap = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -1.0f * TABLE_ROW_GAP * 1.5f)); 
		tableColumnGap = glm::translate(tableRowGap, glm::vec3(3 * TABLE_COLUMN_GAP, 0.0f, 0.0f)); 
		legGapSepTable = glm::translate(identityMatrix, glm::vec3(-TABLE_COLUMN_GAP * 0.05f - TABLE_LEG_WIDTH, 3.0f, 0.0f)); 
		teacher = legGapSepTable * tableColumnGap * teacher; 
		shader.setMat4("model", teacher); 
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // front left leg

		translate = glm::translate(identityMatrix, glm::vec3(TABLE_BASE_WIDTH-0.04f, 0.0f, 0.0f));
		teacher = translate * teacher;
		shader.setMat4("model", teacher);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // front right leg

		translate = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -1.0f * TABLE_BASE_WIDTH + 0.04f));
		teacher = translate * teacher;
		shader.setMat4("model", teacher);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // back right leg

		translate = glm::translate(identityMatrix, glm::vec3(-1.0f * TABLE_BASE_WIDTH + 0.04f, 0.0f, 0.0f));
		teacher = translate * teacher;
		shader.setMat4("model", teacher);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // back right leg

		
		// locker
		scale = glm::scale(identityMatrix, glm::vec3(1.2f, 20.0f, 2.0f));
		translate = glm::translate(identityMatrix, glm::vec3(TABLE_ROW_GAP+0.0f, -0.6f, 0.0f));
		locker = translate * scale * locker;
		shader.setVec3("color", 0.29f, 0.0f, 0.294f);
		shader.setMat4("model", locker);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // locker

		// floor
		scale = glm::scale(identityMatrix, glm::vec3(1.2f, 0.1f, 1.2f));
		translate = glm::translate(identityMatrix, glm::vec3(-3.0f * TABLE_BASE_WIDTH, -0.605f, -4.5f * TABLE_BASE_WIDTH));
		flr = translate * scale * flr;
		shader.setVec3("color", 0.698f, 0.698f, 0.698f);		
		tmpflr = flr;
		glm::mat4 tilesColumnTranslate;
		glm::mat4 tilesRowTranslate; 
		for (int i = 0; i < 11; i++) {
			tilesRowTranslate = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, i * (FLOOR_TILES_WIDTH + FLOOR_TILES_GAP * 3.0f)));
			tmpflr = tilesRowTranslate * flr;
			for (int j = 0; j < 11; j++) {
				if (j > 0) {
					tilesColumnTranslate = glm::translate(identityMatrix, glm::vec3(FLOOR_TILES_WIDTH + FLOOR_TILES_GAP, 0.0f, 0.0f));
					tmpflr = tilesColumnTranslate * tmpflr;
				}
				shader.setMat4("model", tmpflr);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // floor
			}
		}
		
		// walls

		rotate = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		translate = glm::translate(identityMatrix, glm::vec3(-0.6f * FLOOR_TILES_WIDTH, 1.6f * FLOOR_TILES_WIDTH, 0.0f));
		flr = translate * rotate * flr;
		shader.setVec3("color", 1.0f, 0.84f, 0.70f);
		shader.setMat4("model", flr);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // left wall

		// left wall
		for (int i = 0; i < 12; i++) {
			tilesRowTranslate = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, i * (FLOOR_TILES_WIDTH)));
			tmpflr = tilesRowTranslate * flr;
			for (int j = 0; j < 6; j++) {
				if (j > 0) {
					tilesColumnTranslate = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f * FLOOR_TILES_WIDTH, 0.0f));
					tmpflr = tilesColumnTranslate * tmpflr;
				}
				shader.setMat4("model", tmpflr);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // left wall
			}		
		}

		// right wall
		translate = glm::translate(identityMatrix, glm::vec3(10.0f * (FLOOR_TILES_WIDTH) + 2.0f * FLOOR_TILES_GAP * 3.0f, 0.0f, 0.0f));
		flr = translate * flr;
		for (int i = 0; i < 12; i++) {
			tilesRowTranslate = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, i * (FLOOR_TILES_WIDTH)));
			tmpflr = tilesRowTranslate * flr;
			for (int j = 0; j < 6; j++) {
				if (j > 0) {
					tilesColumnTranslate = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f * FLOOR_TILES_WIDTH, 0.0f));
					tmpflr = tilesColumnTranslate * tmpflr;
				}
				shader.setMat4("model", tmpflr);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // left wall
			}
		}

		// front wall
		shader.setVec3("color", 0.925f, 0.91f, 0.84f);
		rotate = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		translate = glm::translate(identityMatrix, glm::vec3(12.5f * (FLOOR_TILES_WIDTH), 0.0f, 4.0f * (FLOOR_TILES_WIDTH)));
		flr = translate * rotate * flr;
		board = flr;
		for (int i = 0; i < 6; i++) {
			tilesRowTranslate = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f * i * (FLOOR_TILES_WIDTH), 0.0f));
			tmpflr = tilesRowTranslate * flr;
			for (int j = 0; j < 12; j++) {
				if (j > 0) {
					tilesColumnTranslate = glm::translate(identityMatrix, glm::vec3(-1.0f * FLOOR_TILES_WIDTH, 0.0f, 0.0f));
					tmpflr = tilesColumnTranslate * tmpflr;
				}
				shader.setMat4("model", tmpflr);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // left wall
			}
		}

		// back wall
		shader.setVec3("color", 1.0f, 1.0f, 1.0f);		
		translate = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, 4.0f * (FLOOR_TILES_WIDTH) * 2.8f));
		flr = translate * flr;
		for (int i = 0; i < 6; i++) {
			tilesRowTranslate = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f * i * (FLOOR_TILES_WIDTH), 0.0f));
			tmpflr = tilesRowTranslate * flr;
			for (int j = 0; j < 12; j++) {
				if (j > 0) {
					tilesColumnTranslate = glm::translate(identityMatrix, glm::vec3(-1.0f * FLOOR_TILES_WIDTH, 0.0f, 0.0f));
					tmpflr = tilesColumnTranslate * tmpflr;
				}
				shader.setMat4("model", tmpflr);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // left wall
			}
		}

		// ceiling
		shader.setVec3("color", 1.0f, 1.0f, 1.0f);
		rotate = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		translate = glm::translate(identityMatrix, glm::vec3(-7.0f, 7.5f, -3.0f * (FLOOR_TILES_WIDTH) * 2.8f));
		flr = translate * rotate * flr;
		for (int i = 0; i < 12; i++) {
			tilesRowTranslate = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, i * (FLOOR_TILES_WIDTH)));
			tmpflr = tilesRowTranslate * flr;
			for (int j = 0; j < 11; j++) {
				if (j > 0) {
					tilesColumnTranslate = glm::translate(identityMatrix, glm::vec3(FLOOR_TILES_WIDTH, 0.0f, 0.0f));
					tmpflr = tilesColumnTranslate * tmpflr;
				}
				shader.setMat4("model", tmpflr);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // floor
			}
		}

		// board
		shader.setVec3("color", 0.0f, 0.0f, 0.0f);
		translate = glm::translate(identityMatrix, glm::vec3(-7.0f * FLOOR_TILES_WIDTH, -2.0f * FLOOR_TILES_WIDTH, 0.05f));
		board = translate * board;
		shader.setMat4("model", board);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // board
		for (int i = 0; i < 2; i++) {
			tilesRowTranslate = glm::translate(identityMatrix, glm::vec3(0.0f, -1.0f * i * (FLOOR_TILES_WIDTH), 0.0f));
			tmpflr = tilesRowTranslate * board;
			for (int j = 0; j < 5; j++) {
				if (j > 0) {
					tilesColumnTranslate = glm::translate(identityMatrix, glm::vec3(FLOOR_TILES_WIDTH, 0.0f, 0.0f));
					tmpflr = tilesColumnTranslate *tmpflr;
				}
				shader.setMat4("model", tmpflr);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // floor
			}
		}

		// fan
		scale = glm::scale(identityMatrix, glm::vec3(0.2f, 1.0f, 0.2f));
		translate = glm::translate(identityMatrix, glm::vec3(1.6f, 2.5f, 0.0f));
		fan = translate * scale * fan;		
		shader.setVec3("color", 0.47f, 0.23f, 0.027f);
		shader.setMat4("model", fan);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // fan center

		scale = glm::scale(identityMatrix, glm::vec3(1.2f, 1.4f, 1.2f));
		translate = glm::translate(identityMatrix, glm::vec3(-0.37f, -0.9f, 0.0f));
		glm::mat4 fan2 = translate * scale * fan;
		shader.setVec3("color", 1.0f, 0.9f, 0.9f);
		shader.setMat4("model", fan2);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // fan center

		scale = glm::scale(identityMatrix, glm::vec3(1.0f, 0.3f, 7.0f));
		translate = glm::translate(identityMatrix, glm::vec3(0.0f, 1.8f, 0.9f));
		glm::mat4 arm = translate * scale * fan;
		shader.setVec3("color", 0.0f, 0.0f, 0.4f);
		shader.setMat4("model", arm);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // fan arm

		glm::mat4 tmp;
		translate = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -1.05f));
		tmp = translate * arm;
		shader.setMat4("model", tmp);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // fan arm
		
		scale = glm::scale(identityMatrix, glm::vec3(0.5f, 1.0f, 0.1f));
		translate = glm::translate(identityMatrix, glm::vec3(0.925f, 0.005f, -0.005f));
		glm::mat4 armc = translate * scale * arm;
		shader.setVec3("color", 1.0f, 1.0f, 1.0f);
		shader.setMat4("model", armc);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // fan arm-center connector
		
		translate = glm::translate(identityMatrix, glm::vec3(0.0f, 0.0f, -0.2f));
		tmp = translate * armc;
		shader.setMat4("model", tmp);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // fan arm-center connector

		shader.setVec3("color", 0.0f, 0.0f, 0.4f);
		rotate = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		translate = glm::translate(identityMatrix, glm::vec3(1.9f, 0.0f, 1.8f));		
		tmp = translate * rotate * arm;		
		shader.setMat4("model", tmp);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // fan arm

		rotate = glm::rotate(identityMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		translate = glm::translate(identityMatrix, glm::vec3(1.83f, 0.0f, -1.93f));		
		tmp = translate * rotate * arm;		
		shader.setMat4("model", tmp);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // fan arm

		rotate = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		translate = glm::translate(identityMatrix, glm::vec3(1.9f, 0.0f, 1.8f));		
		tmp = translate * rotate * armc;
		shader.setVec3("color", 1.0f, 1.0f, 1.0f);
		shader.setMat4("model", tmp);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // fan arm-center connector

		translate = glm::translate(identityMatrix, glm::vec3(-0.15f, 0.0f, 0.0f));
		tmp = translate * tmp;
		shader.setVec3("color", 1.0f, 1.0f, 1.0f);
		shader.setMat4("model", tmp);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // fan arm-center connector

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

	float cameraSpeed = static_cast<float>(2.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {		
		camera.RotateAroundAxis(1, 5.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {		
		camera.RotateAroundAxis(2, 5.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {		
		camera.RotateAroundAxis(3, 3.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		camera.Orbit();
	}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		camera.ResetPosition();
	}

}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;
}
#pragma once
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BufferManager.h"
#include "shader.h"

using namespace std;

class Table
{
public:

	vector<float> base_vertices = {
		// base
		0.0f, 0.0f, 0.0f,  // 0
		0.3f, 0.0f, 0.0f,  // 1
		0.3f, 0.0f, -0.3f, // 2
		0.0f, 0.0f, -0.3f,  // 3
		0.0f, 0.05f, 0.0f,  // 4
		0.3f, 0.05f, 0.0f,  //5
		0.3f, 0.05f, -0.3f,  // 6
		0.0f, 0.05f, -0.3f,  // 7

		// left front leg
		//0.0f, 0.0f, 0.0f, // 8
		//0.02f, 0.0f, 0.0f, // 9
		//0.0f, -0.3f, 0.0f, // 10
		//0.02f, -0.3f, 0.0f, // 11
		//0.0f, 0.0f, -0.02f, // 12
		//0.02f, 0.0f, -0.02f, // 13
		//0.0f, -0.3f,-0.02f, // 14
		//0.02f, -0.3f, -0.02f, // 15
	};

	// base color = 1.0f, 0.0f, 0.0f,
	// leg color = 0.78f, 0.62f, 0.01f,

	vector<unsigned int> base_indices = {
		// base
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
		// left front leg
		/*10, 9, 8,
		11, 9, 10,*/

	};

	//BufferManager buffer;
	float table_base_width = 0.3f, table_leg_width = 0.2f;
	glm::mat4 identityMatrix = glm::mat4(1.0f);
	glm::mat4 translate, scale, rotate, model, frontLeftLeg, frontRightLeg, backLeftLeg, backRightLeg;

	Table() {
		//buffer.manageBuffer(1, base_vertices, base_indices);
		translate = scale = rotate = model = frontLeftLeg = frontRightLeg = backLeftLeg = backRightLeg = glm::mat4(1.0f);
	}

	void setTranslationMatrix (float tx, float ty, float tz) {
		translate = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
		model = translate * model;
	}

	void setRotationMatrix (float anglex, float angley, float anglez, int first = 2, int sec = 1, int third = 0) {
		vector <glm::mat4> rotations;
		rotations[0] = glm::rotate(identityMatrix, glm::radians(anglex), glm::vec3(1.0f, 0.0f, 0.0f));
		rotations[1] = glm::rotate(identityMatrix, glm::radians(angley), glm::vec3(0.0f, 1.0f, 0.0f));
		rotations[2] = glm::rotate(identityMatrix, glm::radians(anglez), glm::vec3(0.0f, 0.0f, 1.0f));		
		rotate = rotations[third] * rotations[sec] * rotations[first];
		model = rotate * model;
	}

	void setScaleMatrix(float sx, float sy, float sz) {
		scale = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
		model = scale * model;
	}

	vector<glm::mat4> getModelMatrices() {
		vector<glm::mat4> models;
		models.push_back(model);
		rotate = glm::rotate(identityMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		scale = glm::scale(identityMatrix, glm::vec3(0.067f, 6.0f, 0.067f));
		frontLeftLeg = scale * rotate * model;
		models.push_back(frontLeftLeg);
		translate = glm::translate(identityMatrix, glm::vec3(table_base_width - table_leg_width, 0, 0));
		frontRightLeg = translate * frontLeftLeg;
		models.push_back(frontRightLeg);
		translate = glm::translate(identityMatrix, glm::vec3(0, 0, -1 * (table_base_width - table_leg_width)));
		backLeftLeg = translate * frontLeftLeg;
		models.push_back(backLeftLeg);
		translate = glm::translate(identityMatrix, glm::vec3(table_base_width - table_leg_width, 0, 0));
		backRightLeg = translate * backLeftLeg;
		models.push_back(backRightLeg);
		return models;
	}

	vector<float> getVertices() {
		return base_vertices;
	}

	vector<unsigned int> getIndices() {
		return base_indices;
	}

	/*void draw (float x_val, float y_val, float z_val) {		
		buffer.bindVA();
		glDrawElements(GL_TRIANGLES, sizeof(base_indices), GL_UNSIGNED_INT, 0);
		buffer.unbindVA();
	}*/	

};

	



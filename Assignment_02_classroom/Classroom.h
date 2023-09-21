#pragma once
#include <iostream>
#include <vector>

using namespace std;

class Classroom
{
public:

	vector<float> wallVertices = {		
		0.0f, 0.0f, 0.0f,
		0.9f, 0.0f, 0.0f,
		0.9f, 0.0f, -0.9f,
		0.0f, 0.0f, -0.9f,
		0.9f, 0.4f, 0.0f,
		0.0f, 0.4f, 0.0f,
		0.9f, 0.4f, -0.9f,
		0.0f, 0.4f, -0.9f,
	};

	vector<float> wallIndices = {
		// floor
		0, 1, 2,
		
	};



};


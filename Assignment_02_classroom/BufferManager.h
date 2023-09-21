#pragma once
#include <vector>
#include <glad/glad.h>
using namespace std;

class BufferManager
{
public:
    unsigned int VBO, VAO, EBO;

    BufferManager() {}

	void manageBuffer(int num_attr, vector<float> vertices, vector<unsigned int> indices) {        

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            //return -1;
        }
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

        int stride = num_attr * 3;
        for (int i = 0; i < num_attr; i++) {
            glVertexAttribPointer(i, 3, GL_FLOAT, GL_TRUE, stride * sizeof(float), (void*)0);
            glEnableVertexAttribArray(i);
        }

        //// position attribute
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, stride * sizeof(float), (void*)0);
        //glEnableVertexAttribArray(0);

        //color attribute
       /* glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (3*sizeof(float)));
        glEnableVertexAttribArray(1);*/

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
	}

    void bindVA() {
        glBindVertexArray(VAO);
    }

    void unbindVA() {
        glBindVertexArray(0);
    }

    void deleteBuffers() {
        glDeleteVertexArrays(1, &VAO); 
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};


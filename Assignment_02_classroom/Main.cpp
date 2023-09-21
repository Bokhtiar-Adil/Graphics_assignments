#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "BufferManager.h"
#include "camera.h"
#include "Table.h"
#include "basic_camera.h"

#include <iostream>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int WIN_WIDTH = 1000;
int WIN_HEIGHT = 600;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Classroom", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

  
    Shader shader("vsSrc.vs", "fsSrc.fs");
    
    // Table
    //Table table;

    //float table_vertices[] = {
    //    // base
    //    0.0f, 0.0f, 0.0f,  // 0
    //    0.3f, 0.0f, 0.0f,  // 1
    //    0.3f, 0.0f, -0.3f, // 2
    //    0.0f, 0.0f, -0.3f,  // 3
    //    0.0f, 0.05f, 0.0f,  // 4
    //    0.3f, 0.05f, 0.0f,  //5
    //    0.3f, 0.05f, -0.3f,  // 6
    //    0.0f, 0.05f, -0.3f,  // 7
    //};

    float table_vertices[] = {
        0.0f, 0.0f, 0.0f, 
        0.5f, 0.0f, 0.0f, 
        0.5f, 0.5f, 0.0f, 
        0.0f, 0.5f, 0.0f, 

        0.5f, 0.0f, 0.0f, 
        0.5f, 0.5f, 0.0f, 
        0.5f, 0.0f, 0.5f, 
        0.5f, 0.5f, 0.5f, 

        0.0f, 0.0f, 0.5f, 
        0.5f, 0.0f, 0.5f, 
        0.5f, 0.5f, 0.5f, 
        0.0f, 0.5f, 0.5f, 

        0.0f, 0.0f, 0.5f, 
        0.0f, 0.5f, 0.5f, 
        0.0f, 0.5f, 0.0f, 
        0.0f, 0.0f, 0.0f, 

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.0f, 
        0.0f, 0.5f, 0.0f, 
        0.0f, 0.5f, 0.5f, 

        0.0f, 0.0f, 0.0f, 
        0.5f, 0.0f, 0.0f, 
        0.5f, 0.0f, 0.5f, 
        0.0f, 0.0f, 0.5f, 
    };

    //float table_indices[] = {
    //    // base
    //    0, 1, 3,
    //    1, 2, 3,
    //    4, 5, 7,
    //    5, 6, 7,
    //    0, 1, 5,
    //    0, 5, 4,
    //    1, 2, 6,
    //    1, 6, 5,
    //    3, 2, 6,
    //    3, 6, 7,
    //    0, 3, 7,
    //    0, 7, 4,
    //    // left front leg
    //    /*10, 9, 8,
    //    11, 9, 10,*/
    //};

    unsigned int table_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    float table_base_width = 0.3f, table_leg_width = 0.2f;
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translate, scale, rotatex, rotatey, rotatez, model, frontLeftLeg, frontRightLeg, backLeftLeg, backRightLeg;
    model = glm::mat4(1.0f);
    /*translate = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
    rotatex = glm::rotate(identityMatrix, glm::radians(anglex), glm::vec3(1.0f, 0.0f, 0.0f));
    rotatey = glm::rotate(identityMatrix, glm::radians(angley), glm::vec3(0.0f, 1.0f, 0.0f));
    rotatez = glm::rotate(identityMatrix, glm::radians(anglez), glm::vec3(0.0f, 0.0f, 1.0f));
    scale = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));*/
    shader.use();

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIN_WIDTH / (float)WIN_HEIGHT, 0.1f, 100.0f);
    //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    // camera/view transformation
    //glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 view = basic_camera.createViewMatrix();
    shader.setMat4("view", view);

    shader.setMat4("transform", model);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(table_vertices), table_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(table_indices), table_indices, GL_STATIC_DRAW);

    int stride = 3;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        // processInput(window);

        shader.use();
        shader.setMat4("transform", model);
        shader.setVec3("color", 1.0f, 0.0f, 0.0f);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window) {}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {}
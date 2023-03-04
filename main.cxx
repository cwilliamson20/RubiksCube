#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using std::cout;
#define GLFW_INCLUDE_NONE
#include <GL/glew.h>    // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
// avoid having to use glm:: all the time
#include <glm/glm.hpp>
using namespace glm;
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale
#include <glm/gtc/type_ptr.hpp>

GLFWwindow* setUpAndCreateWindow() {
    if (!glfwInit()) {
        // initialization of GLFW failed
        cout << "Could not start GLFW\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // create the window
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Rubik's Cube Simulator", NULL, NULL);
    


    // make sure there weren't any error creating the window
    if (!window) {
        cout << "Could not create the window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
    }
    return window;
}


int main() {
    GLFWwindow *window = setUpAndCreateWindow();
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // make the vao and set it as the current one
    GLuint vertex_array_ID;
    glGenVertexArrays(1, &vertex_array_ID);
    glBindVertexArray(vertex_array_ID);

    GLuint program_id = LoadShaders( "vertexshader.glsl", "fragmentshader.glsl" );

    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,   // top left 
        1.0f, 1.0f, 0.0f
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3,    // second triangle
        0, 1, 4
    };  

    float colors[] = {
        1.0f, 0.0f, 0.0f,   // 0 = red
        0.0f, 1.0f, 0.0f,   // 1 = green
        0.0f, 0.0f, 1.0,    // 2 = blue
        1.0f, 0.0f, 1.0,    // 3 = purple
        1.0f, 1.0f, 1.0     // 4 = white
    };


    // make a uint that will identify our vertex buffer
    GLuint vertex_buffer;
    // generate 1 buffer, put the resulting identifier in vertex_buffer
    glGenBuffers(1, &vertex_buffer);
    // bind it to the array buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    // give our vertices to OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // tell OpenGL how to look at this data when drawing
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);


    // store the indices of the triangles in an Element Buffer Object (EBO)
    // which is basically a vertex buffer object that stores indices to decide what vertices to draw
    // create the element buffer object    
    GLuint EBO;
    glGenBuffers(1, &EBO);
    // bind the EBO and copy the indices into the buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // store the colors of each vertex of the triangle
    // make a uint to identify the color buffer
    GLuint color_buffer;
    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    // tell OpenGL how to look at this data when drawing
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);


    // enable to draw in wireframe mode and see the edges of the triangles
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // window will close with alt + F4, X button, or escape key
    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        // clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(program_id);

        glBindVertexArray(vertex_array_ID);
        // Draw the triangle !
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        // swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean up VBO and shaders
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteProgram(program_id);
    glDeleteVertexArrays(1, &vertex_array_ID);

    glfwDestroyWindow(window);
    glfwTerminate();
}
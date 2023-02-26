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
#include <glm/gtc/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale
#include "shader.h"
#include "controls.h"

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
    GLFWwindow* window = glfwCreateWindow(2048, 1536, "Rubik's Cube Simulator", NULL, NULL);
    
    // Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Cull triangles whose normal is not towards the camera
    glEnable(GL_CULL_FACE);

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

    // make the vao and set it as the current one
    GLuint vertex_array_ID;
    glGenVertexArrays(1, &vertex_array_ID);
    glBindVertexArray(vertex_array_ID);

    GLuint program_id = LoadShaders( "vertexshader.glsl", "fragmentshader.glsl" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(program_id, "MVP");

    // test array of three vertices that will form one cube
    // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    // One color for each vertex. They were generated randomly.
    static const GLfloat g_color_buffer_data[] = {
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

    // make a uint that will identify our vertex buffer
    GLuint vertex_buffer;
    // generate 1 buffer, put the resulting identifier in vertex_buffer
    glGenBuffers(1, &vertex_buffer);
    // bind it to the array buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    // give our vertices to OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // make a uint that will identify our color buffer 
    GLuint color_buffer;
    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

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

        computeMatricesFromInputs(window);
        mat4 ProjectionMatrix = getProjectionMatrix();
        mat4 ViewMatrix = getViewMatrix();
        mat4 ModelMatrix = mat4(1.0);
        mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        
        // Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // set up the vertex and color buffers
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer);

        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );
        
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 12*3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

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
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

// TODO: make these be held in their own file for easy updating instead of sitting up here
vec3 camera_pos = vec3(0.0f, 0.0f, 3.0f);
vec3 camera_front = vec3(0.0f, 0.0f, -1.0f);
vec3 camera_up = vec3(0.0f, 1.0f, 0.0f);

float delta_time = 0.0f;	// Time between current frame and last frame
float last_frame = 0.0f; // Time of last frame

float vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f, 

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f, 
     0.5f, -0.5f, -0.5f, 
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f, 

    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f, 
     0.5f, -0.5f,  0.5f,  
     0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f, 
    -0.5f, -0.5f, -0.5f,  

    -0.5f,  0.5f, -0.5f, 
     0.5f,  0.5f, -0.5f, 
     0.5f,  0.5f,  0.5f, 
     0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f, -0.5f,  
};

int NUM_TRIANGLES = 1;  // update this to draw different numbers of triangles
unsigned int indices[] = {  // note that we start from 0!
    0, 4, 5,
    0, 1, 3,   
    1, 2, 3,    
    0, 1, 4, 
};  

float colors[] = {
    1.0f, 0.0f, 0.0f,   // red
    0.0f, 1.0f, 0.0f,   // green
    0.0f, 0.0f, 1.0,    // blue
    1.0f, 0.0f, 1.0,    // purple
    1.0f, 1.0f, 1.0,    // white
    1.0f, 1.0f, 0.0,    // yellow
    1.0f, 0.0f, 0.0f,   // red
    0.0f, 1.0f, 0.0f,   // green
    0.0f, 0.0f, 1.0,    // blue
    1.0f, 0.0f, 1.0,    // purple
    1.0f, 1.0f, 1.0,    // white
    1.0f, 1.0f, 0.0,    // yellow
    1.0f, 0.0f, 0.0f,   // red
    0.0f, 1.0f, 0.0f,   // green
    0.0f, 0.0f, 1.0,    // blue
    1.0f, 0.0f, 1.0,    // purple
    1.0f, 1.0f, 1.0,    // white
    1.0f, 1.0f, 0.0,    // yellow
    1.0f, 0.0f, 0.0f,   // red
    0.0f, 1.0f, 0.0f,   // green
    0.0f, 0.0f, 1.0,    // blue
    1.0f, 0.0f, 1.0,    // purple
    1.0f, 1.0f, 1.0,    // white
    1.0f, 1.0f, 0.0,    // yellow
    1.0f, 0.0f, 0.0f,   // red
    0.0f, 1.0f, 0.0f,   // green
    0.0f, 0.0f, 1.0,    // blue
    1.0f, 0.0f, 1.0,    // purple
    1.0f, 1.0f, 1.0,    // white
    1.0f, 1.0f, 0.0,    // yellow
    1.0f, 0.0f, 0.0f,   // red
    0.0f, 1.0f, 0.0f,   // green
    0.0f, 0.0f, 1.0,    // blue
    1.0f, 0.0f, 1.0,    // purple
    1.0f, 1.0f, 1.0,    // white
    1.0f, 1.0f, 0.0,    // yellow
    1.0f, 0.0f, 0.0f,   // red
    0.0f, 1.0f, 0.0f,   // green
    0.0f, 0.0f, 1.0,    // blue
    1.0f, 0.0f, 1.0,    // purple
    1.0f, 1.0f, 1.0,    // white
    1.0f, 1.0f, 0.0,    // yellow
};

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};

GLFWwindow* setUpAndCreateWindow(int width, int height) {
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
    GLFWwindow* window = glfwCreateWindow(width, height, "Rubik's Cube Simulator", NULL, NULL);
    


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

void setUpBuffersAndEBO(GLuint vertex_buffer, GLuint EBO, GLuint color_buffer) {
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
    glGenBuffers(1, &EBO);
    // bind the EBO and copy the indices into the buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // store the colors of each vertex of the triangle
    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    // tell OpenGL how to look at this data when drawing
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
}

void processInput(GLFWwindow *window)
{
    const float camera_speed = 2.5 * delta_time; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
        camera_pos += camera_speed * camera_front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
        camera_pos -= camera_speed * camera_front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_pos -= normalize(cross(camera_front, camera_up)) * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_pos += normalize(cross(camera_front, camera_up)) * camera_speed;

    // cout << "cam speed = " << camera_speed << ", cam front = " << camera_front.x << camera_front.y << camera_front.z << "\n";
    // cout << "cam pos = " << camera_pos.x << ", " << camera_pos.y << ", " << camera_pos.z << "\n";
    // cout << "cam front = " << camera_front.x << ", " << camera_front.y << ", " << camera_front.z << "\n";
    // cout << "cross = " << normalize(cross(camera_front, camera_up)).x << ", " << cross(camera_front, camera_up).y << ", " << cross(camera_front, camera_up).z << "\n";
}

void setUpMVPMatrices(GLuint program_id, int width, int height, vec3 model_translate, float temp_angle) {
    mat4 model = mat4(1.0f);
    model = translate(model, model_translate);
    model = rotate(model, radians(temp_angle), vec3(1.0f, 0.3f, 0.5f));

    mat4 view = mat4(1.0f);
    // view = translate(view, vec3(0.0f, 0.0f, -3.0f));
    // use view to enable user camera movement
    view = lookAt(camera_pos, camera_pos + camera_front, camera_up);

    mat4 projection;
    projection = perspective(radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

    // send to the shader
    GLuint model_loc = glGetUniformLocation(program_id, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, value_ptr(model));
    GLuint view_loc = glGetUniformLocation(program_id, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, value_ptr(view));
    GLuint projection_loc = glGetUniformLocation(program_id, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection));
}

int main() {
    int window_width = 800;
    int window_height = 600;
    GLFWwindow *window = setUpAndCreateWindow(window_width, window_height);
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // make the vao and set it as the current one
    GLuint vertex_array_ID;
    glGenVertexArrays(1, &vertex_array_ID);
    glBindVertexArray(vertex_array_ID);

    GLuint program_id = LoadShaders("vertexshader.glsl", "fragmentshader.glsl");
    // Use our shader
    glUseProgram(program_id);

    // set up buffers and EBO
    GLuint vertex_buffer, EBO, color_buffer;
    setUpBuffersAndEBO(vertex_buffer, EBO, color_buffer);

    // enable to draw in wireframe mode and see the edges of the triangles
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    // glDepthFunc(GL_LESS);
    // window will close with alt + F4, X button, or escape key
    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        // calculate the delta time since the last frame
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // process input
        processInput(window);

        // clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vertex_array_ID);
        
        // Draw the triangles
        // use glDrawElements if using the EBO, glDrawArrays if using just the entire vertices array
        // glDrawElements(GL_TRIANGLES, NUM_TRIANGLES * 3, GL_UNSIGNED_INT, 0);
        // for drawing 10 boxes that are the same but differ in position
        // make a loop that renders 10 times with a different model matrix each time
        for (int x = 0; x < 10; x++) {
            setUpMVPMatrices(program_id, window_width, window_height, cubePositions[x], 20*x);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

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
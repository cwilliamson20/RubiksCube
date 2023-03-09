#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using std::cout;
#define GLFW_INCLUDE_NONE
#include <GL/glew.h>    // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
// avoid having to use  all the time
#include <glm/glm.hpp>
using namespace glm;
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>  // translate, rotate, scale
#include <glm/gtc/type_ptr.hpp>

vec3 camera_pos = vec3(0.0f, 0.0f, 3.0f);
vec3 camera_front = vec3(0.0f, 0.0f, 0.0f);
vec3 camera_up = vec3(0.0f, 1.0f, 0.0f);
float cam_rotation_angle = 0;
float rotation_radius = 10.0f;
float speed_coefficient = 2.5;
float cam_height = 0.0;

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

// index in array corresponds to the position held in the cube object
vec3 cube_translates[] = {
    // height depth
    // top front row
    vec3(-1.0f,  1.0f,  1.0f),  
    vec3( 0.0f,  1.0f,  1.0f), 
    vec3( 1.0f,  1.0f,  1.0f), 
    // middle front row
    vec3(-1.0f,  0.0f,  1.0f), 
    vec3( 0.0f,  0.0f,  1.0f), 
    vec3( 1.0f,  0.0f,  1.0f), 
    // bottom front row 
    vec3(-1.0f, -1.0f,  1.0f),   
    vec3( 0.0f, -1.0f,  1.0f),   
    vec3( 1.0f, -1.0f,  1.0f), 
    
    // top middle row
    vec3(-1.0f,  1.0f,  0.0f),  
    vec3( 0.0f,  1.0f,  0.0f), 
    vec3( 1.0f,  1.0f,  0.0f), 
    // middle middle row
    vec3(-1.0f,  0.0f,  0.0f), 
    vec3( 0.0f,  0.0f,  0.0f), 
    vec3( 1.0f,  0.0f,  0.0f), 
    // bottom middle row 
    vec3(-1.0f, -1.0f,  0.0f),   
    vec3( 0.0f, -1.0f,  0.0f),   
    vec3( 1.0f, -1.0f,  0.0f),    
    
    // top back row
    vec3(-1.0f,  1.0f, -1.0f),  
    vec3( 0.0f,  1.0f, -1.0f), 
    vec3( 1.0f,  1.0f, -1.0f), 
    // middle back row
    vec3(-1.0f,  0.0f, -1.0f), 
    vec3( 0.0f,  0.0f, -1.0f), 
    vec3( 1.0f,  0.0f, -1.0f), 
    // bottom back row 
    vec3(-1.0f, -1.0f, -1.0f),   
    vec3( 0.0f, -1.0f, -1.0f),   
    vec3( 1.0f, -1.0f, -1.0f),     
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 36, vertices, GL_STATIC_DRAW);
    // tell OpenGL how to look at this data when drawing
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // store the colors of each vertex of the triangle
    // starts as NULL because each cube will stream it's color data into it later
    // https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 36, NULL, GL_STATIC_DRAW);
    // tell OpenGL how to look at this data when drawing
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
}

void processInput(GLFWwindow *window)
{
    const float camera_speed = speed_coefficient * delta_time; // adjust accordingly
    // W is get closer, so shrink down rotation radius
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
        rotation_radius -= camera_speed;
    // get further away
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
        rotation_radius += camera_speed;
    // rotate cube left to right
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam_rotation_angle -= camera_speed;
    // rotate cube right to left
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam_rotation_angle += camera_speed;
    // increase camera speed
    if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) 
        speed_coefficient += delta_time;
    // decrease camera speed
    if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && speed_coefficient > delta_time) 
        speed_coefficient -= delta_time;
    // raise camera up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
        cam_height += camera_speed;
    // lower camera down
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cam_height -= camera_speed;
}

void setUpMVPMatrices(GLuint program_id, int width, int height, vec3 model_translate) {
    mat4 model = mat4(1.0f);
    model = translate(model, model_translate);
    // model = rotate(model, radians(temp_angle), vec3(1.0f, 0.3f, 0.5f));

    mat4 view = mat4(1.0f);
    // use view to enable user camera movement
    camera_pos = vec3(sin(cam_rotation_angle) * rotation_radius, cam_height, cos(cam_rotation_angle) * rotation_radius);
    view = lookAt(camera_pos, camera_front, camera_up);

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

class Cube {
    // this is the blueprint for one cube on the rubik's cube out of 27 cubes
    public:
        int position;   // from 0 to 26, detailing which of the parts of the cube it is
        GLuint cube_vertex_buffer;
        GLuint cube_color_buffer;
        float cube_colors[108];

    Cube() {   
        // initialize all colors for all 36 vertices to 0
        for (int x = 0; x < 3 * 36; x++) {
            cube_colors[x] = 0.5;
        }
    }

    void activateCubeColors() {
        // binds this cube's colors as the active color VBO
        glGenBuffers(1, &cube_color_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, cube_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 36, cube_colors, GL_STATIC_DRAW);
        // tell OpenGL how to look at this data when drawing
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
    }
};
int main() {
    int window_width = 800;
    int window_height = 600;
    GLFWwindow *window = setUpAndCreateWindow(window_width, window_height);
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // make 27 cube objects for the 27 parts of the overall rubik's cube
    Cube cube_list[27];
    for (int x = 0; x < 27; x++) {
        cube_list[x].position = x;
        // cube_list[x].setUpCubeBuffers();
    }
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
        
        // draw all of the cubes from the cube list
        for (int x = 0; x < 15; x++) {
            cube_list[x].activateCubeColors();
            setUpMVPMatrices(program_id, window_width, window_height, cube_translates[x]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}
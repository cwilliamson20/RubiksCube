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
#include <unistd.h>


vec3 camera_pos = vec3(0.0f, 0.0f, 3.0f);
vec3 camera_front = vec3(0.0f, 0.0f, 0.0f);
vec3 camera_up = vec3(0.0f, 1.0f, 0.0f);
float cam_rotation_angle = 0;
float rotation_radius = 10.0f;
float speed_coefficient = 5.0;
float cam_height = 0.0;

float delta_time = 0.0f;	// Time between current frame and last frame
float last_frame = 0.0f; // Time of last frame
float frames_per_second = 60.0;
float start_time = glfwGetTime();

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

class RotationStatus {
    public:
        bool is_rotating;   // true if there is a rotation in progress, false otherwise
        // index:   0     1      2    3       4      5
        // side:  back, front, left, right, bottom, top
        int rotation_side;  // which side of the cube is being rotated?
        float cur_angle;    // the angle at which the rotating side should be at (in RADIANS)
        int rotation_start_frame;   // the frame this rotation started at
        int frames_per_rotation;    // how many frames should a rotation be? By default set to about 1 second

    RotationStatus(bool r, int r_s, int start_frame) {
        is_rotating = r;
        rotation_side = r_s;
        cur_angle = 0.0f;
        rotation_start_frame = start_frame;
        frames_per_rotation = frames_per_second;
    }

    void update_rotation(int cur_frame) {
        // updates the rotation values based on how many frames have passed since the rotation started
        
        // check if the current rotation should be finished
        if (cur_frame - rotation_start_frame >= frames_per_rotation) {
            // finish the rotation and set this side into the final position
            // TODO: change this to actually change the position values for the cubes? This won't be permanent once is_rotating is false
            cur_angle = radians(-90.0f);
            return;
        }
        cur_angle -= radians((float)1 / frames_per_rotation * 90.0f);
        // cout << "cur_angle = " << cur_angle << "\n";
    }
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

mat4 generateRotationModelMatrix(mat4 in_model, bool is_rotating, int start_position, float rotation_angle) {
    // takes in the model for an individual cube and returns the model matrix
    // that will put it in the right spot if it's currently being rotated
    if (!is_rotating || rotation_angle == 0.0) return in_model;

    // basic idea is to translate to the center of rotation (center of side that is rotating)
    // rotate, then translate back to original position
    // translate needs to be different for each block
    vec3 center_translate = vec3(0.0f, 0.0f, 0.0f);
    // closest to front, top to bottom
    if (start_position == 2)
        center_translate = vec3(0.0f, -1.0f, -1.0f);
    if (start_position == 5)
        center_translate = vec3(0.0f, 0.0f, -1.0f);
    if (start_position == 8)
        center_translate = vec3(0.0f, 1.0f, -1.0f);
    // middle to camera
    if (start_position == 11)
        center_translate = vec3(0.0f, -1.0f, 0.0f);
    if (start_position == 14)
        center_translate = vec3(0.0f, 0.0f, 0.0f);
    if (start_position == 17)
        center_translate = vec3(0.0f, 1.0f, 0.0f);
    // farthest from camera
    if (start_position == 20)
        center_translate = vec3(0.0f, -1.0f, 1.0f);
    if (start_position == 23)
        center_translate = vec3(0.0f, 0.0f, 1.0f);
    if (start_position == 26)
        center_translate = vec3(0.0f, 1.0f, 1.0f);


    if (start_position % 3 == 2) {
    in_model = translate(in_model, center_translate);
    in_model = rotate(in_model, rotation_angle, vec3(1.0, 0.0, 0.0));
    center_translate *= -1;
    in_model = translate(in_model, center_translate);
    }
    return in_model;
}
void setUpMVPMatrices(GLuint program_id, int width, int height, vec3 model_translate, RotationStatus rs, int index) {
    mat4 model = mat4(1.0f);
    model = translate(model, model_translate);
    model = generateRotationModelMatrix(model, true, index, rs.cur_angle);

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

    void setCubeSideColor(int side, float r, float g, float b) {
        // changes the color of one side of this cube
        // 0 = front, 1 = back, 2 = left, 3 = right, 4 = top, 5 = bottom
        // in_colors should be three long, r, g, b
        int offset = side * 18;
        for (int x = 0; x < 18; x+=3) {
            cube_colors[x + offset] = r / 255;
            cube_colors[x + offset + 1] = g / 255;
            cube_colors[x + offset + 2] = b / 255; 
        }
    }

    void printCubeInfo() {
        // prints the current value of this cube for debugging
        cout << "position = " << position << "\n";
        cout << "cube_colors = \n";
        for (int side = 0; side < 6; side++) {
            cout << "side " << side << ": ";
            for (int x = 0; x < 18; x+=3) {
                cout << "(";
                cout << (int) (cube_colors[x + (side * 18)] * 255) << " "; 
                cout << (int) (cube_colors[x + (side * 18) + 1] * 255) << " "; 
                cout << (int) (cube_colors[x + (side * 18) + 2] * 255);
                cout << ") ";
            }
            cout << "\n";
        }
    }
};

class CubeList {
    // class to contain the cubes and make doing operations on all of them easier
    public:
        int num_cubes = 27;
        Cube cubes[27];
    
    CubeList() {
        // set up position values for each cube
        for (int x = 0; x < 27; x++) {
            cubes[x].position = x;
        }
    }

    void setAllCubeSideColor(int side, float r, float g, float b) {
        // sets all the cubes in the cube list's side number to the rgb value
        // index:   0     1      2    3       4      5
        // side:  back, front, left, right, bottom, top
        for (int x = 0; x < num_cubes; x++) {
            cubes[x].setCubeSideColor(side, r, g, b);
        }
    }

    void changeColorPallete(float in_colors[]) {
        // takes in a list of 18 floats an applies that color scheme to the cube
        cout << in_colors[0] << " " << in_colors[1] << " " << in_colors[2] << "\n";
        for (int x = 0; x < 18; x+= 3) {
            cout << x << "\n";
            setAllCubeSideColor(x / 3, in_colors[x], in_colors[x + 1], in_colors[x + 2]);
        }
    }

    void rotateSide() {
        // does an R rotation on the cube from the original camera position
        // cube positions to work on: 2, 5, 8, 11, 14, 17, 20, 23, 26
        // idea: rotate around the center of the middle right cube 
        // translate to that position, rotate, then reverse the translate

        // want a rotation to take one second, which means we have frames_per_second to move it
        // between start position and end position

        
        return;
    }
};

int main() {
    int window_width = 800;
    int window_height = 600;
    GLFWwindow *window = setUpAndCreateWindow(window_width, window_height);
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // make 27 cube objects for the 27 parts of the overall rubik's cube
    CubeList cube_list;

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

    // set up initial default color pallete
    // back, front, left, right, bottom, top
    float default_colors[] = {
        255.0,  133.0,  2.0,    // back = orange
        255.0,  0.0,    0.0,    // front = red
        0.0,    0.0,  255.0,    // left = blue
        0.0,    255.0,  0.0,    // right = green
        255.0,  255.0,255.0,    // bottom = white
        255.0,  255.0,  0.0,    // top = yellow
    };

    cube_list.changeColorPallete(default_colors);

    // set up rotation status tracker
    RotationStatus rs(false, -1, 0);

    // Accept fragment if it closer to the camera than the former one
    // glDepthFunc(GL_LESS);
    // window will close with alt + F4, X button, or escape key
    int num_frames = 0;
    float end_time;
    float frame_start_time = glfwGetTime();
    float frame_end_time;
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
        for (int x = 0; x < 27; x++) {
            cube_list.cubes[x].activateCubeColors();
            setUpMVPMatrices(program_id, window_width, window_height, cube_translates[x], rs, cube_list.cubes[x].position);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // update the rotation status
        rs.update_rotation(num_frames);

        // swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        // this code caps the framerate at the global variable frames_per_second
        frame_end_time = glfwGetTime();
        float change = frame_end_time - frame_start_time;

        if (change < 1 / frames_per_second) {
            usleep(((1 / frames_per_second) - change) * 1000000);
        }
        num_frames ++;
        end_time = glfwGetTime();
        frame_start_time = glfwGetTime();
    }

    cout << "total time = " << end_time - start_time << "\n";
    cout << "total number of frames = " << num_frames << "\n";
    glfwDestroyWindow(window);
    glfwTerminate();
}

// TODO: figure out rotation of one side of the cube
// TODO: make non visible sides of cubes gray so that opposite color doesn't show mid turn
// TODO: make sure that use of sleep function will actually function on all OS's https://www.geeksforgeeks.org/sleep-function-in-cpp/

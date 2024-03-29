#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using std::cout;
#define GLFW_INCLUDE_NONE
#include <GL/glew.h>    // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <glm/glm.hpp>
using namespace glm;
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>  // translate, rotate, scale
#include <glm/gtc/type_ptr.hpp>
#include <unistd.h>
#include "stb_image.h"
#include <cstdlib>


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
int change_texture = 0.0;
std::vector<int> move_queue;
bool change_color = false;
bool solved = true;

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


// index in array corresponds to the cur_position held in the cube object
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

    RotationStatus() {
        is_rotating = false;
        rotation_side = -1;
        cur_angle = 0.0f;
        rotation_start_frame = 0;
        frames_per_rotation = frames_per_second;
    }

    bool updateRotation(int cur_frame) {
        // updates the rotation values based on how many frames have passed since the rotation started
        // check if the current rotation should be finished

        // rotation type indicates which slice of the cube is rotating
                            // back, front,  left,  right, bottom,  top, equator, entire, back', front', left',  right', bottom', top', equator', entire'
        float angle_list[] = {90.0f, -90.0f, 90.0f, -90.0f, 90.0f, -90.0f, -90.0f, -90.0f, -90.0f, 90.0f, -90.0f, 90.0f, -90.0f, 90.0f, 90.0f, 90.0f};
        float slice_angle = angle_list[rotation_side];

        if (!is_rotating) {
            return false;
        }
        if (cur_frame - rotation_start_frame == frames_per_rotation) {
            // finish the rotation and set this side into the final cur_position
            is_rotating = false;
            cur_angle = 0.0f;
            // cout << "just finished the rotation on frame " << cur_frame << "\n";
            return true;
        } else if (cur_frame - rotation_start_frame < frames_per_rotation) {
            cur_angle += radians((float)1 / frames_per_rotation * slice_angle);
            // cout << "in updateRotation, cur_frame = " << cur_frame << "\n";
        }
        return false;
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

    // set up texture to change look of cube
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLfloat value, max_anisotropy = 8.0f; /* don't exceed this value...*/
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, & value);

    value = (value > max_anisotropy) ? max_anisotropy : value;
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
    
    // load and generate the texture
    float texture_coords[] = {
        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f, 
        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f, 

        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f, 
        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f, 
        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f, 
        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f, 
        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f, 
        0.0f, 1.0f, 
        0.0f, 0.0f,
        1.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 36, texture_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(2);  
    glBindTexture(GL_TEXTURE_2D, texture);
    int width, height, nrChannels;
    unsigned char *data;
    data = stbi_load("default_stickered.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

class Cube {
    // this is the blueprint for one cube on the rubik's cube out of 27 cubes
    public:
        int array_position;     // the cubes physical, unchanging location in the cubes array in the cube list
        GLuint cube_vertex_buffer;
        GLuint cube_color_buffer;
        float cube_colors[108];

    Cube() {   
        // initialize all colors for all 36 vertices to 0
        for (int x = 0; x < 3 * 36; x++) {
            cube_colors[x] = 0.0;
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
        cout << "array_position = " << array_position << "\n";
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

    void rotateCubeColorsRight(int right_times) {
        // rotates all the colors on the cube right_times left to right
        // make right rotations as if making right side the bottom (clockwise motion)
        for (int x = 0; x < right_times % 4; x++) {
            float cur_colors[] = {
                    cube_colors[90], cube_colors[91], cube_colors[92],      // original top colors
                    cube_colors[54], cube_colors[55], cube_colors[56],      // original right colors
                    cube_colors[72], cube_colors[73], cube_colors[74],      // original bottom colors
                    cube_colors[36], cube_colors[37], cube_colors[38],      // original left colors
            };

            // multiply all by 255 because colors are stored 0 to 1 but change color function takes in 0 to 255
            for (int x = 0; x < 12; x++) {
                cur_colors[x] *= 255.0f;
            }

            // change cube_colors to have rotated around by one clockwise
            setCubeSideColor(5, cur_colors[9], cur_colors[10], cur_colors[11]);  // set top to left
            setCubeSideColor(2, cur_colors[6], cur_colors[7], cur_colors[8]);    // set left to bottom
            setCubeSideColor(4, cur_colors[3], cur_colors[4], cur_colors[5]);    // set bottom to right
            setCubeSideColor(3, cur_colors[0], cur_colors[1], cur_colors[2]);    // set right to top
        } 
    }

    void rotateCubeColorsDown(int down_times) {
        // rotates all the colors on the cube down_times down
        // make down rotations as if top color comes towards the camera (clockwise if viewed from the right side)
        for (int x = 0; x < down_times % 4; x++) {
            float cur_colors[] = {
                    cube_colors[18], cube_colors[19], cube_colors[20],      // original front colors
                    cube_colors[72], cube_colors[73], cube_colors[74],      // original bottom colors
                    cube_colors[0], cube_colors[1], cube_colors[2],         // original back colors
                    cube_colors[90], cube_colors[91], cube_colors[92],      // original top colors
            };

            // multiply all by 255 because colors are stored 0 to 1 but change color function takes in 0 to 255
            for (int x = 0; x < 12; x++) {
                cur_colors[x] *= 255.0f;
            }

            // change cube_colors to have rotated around by one clockwise
            setCubeSideColor(1, cur_colors[9], cur_colors[10], cur_colors[11]);  // set front to top
            setCubeSideColor(4, cur_colors[0], cur_colors[1], cur_colors[2]);    // set bottom to front
            setCubeSideColor(0, cur_colors[3], cur_colors[4], cur_colors[5]);    // set back to bottom
            setCubeSideColor(5, cur_colors[6], cur_colors[7], cur_colors[8]);    // set top to back
        } 
    }
    void rotateCubeColorsClockwise(int clockwise_times) {
    // rotates all the colors on the cube as if spinning the cube clockwise while keeping the base flat
    for (int x = 0; x < clockwise_times % 4; x++) {
        float cur_colors[] = {
                cube_colors[18], cube_colors[19], cube_colors[20],      // original front colors
                cube_colors[36], cube_colors[37], cube_colors[38],      // original left colors
                cube_colors[0], cube_colors[1], cube_colors[2],         // original back colors
                cube_colors[54], cube_colors[55], cube_colors[56],      // original right colors
        };

        // multiply all by 255 because colors are stored 0 to 1 but change color function takes in 0 to 255
        for (int x = 0; x < 12; x++) {
            cur_colors[x] *= 255.0f;
        }

        // change cube_colors to have rotated around by one clockwise
        setCubeSideColor(1, cur_colors[9], cur_colors[10], cur_colors[11]);  // set front to right
        setCubeSideColor(2, cur_colors[0], cur_colors[1], cur_colors[2]);    // set left to front
        setCubeSideColor(0, cur_colors[3], cur_colors[4], cur_colors[5]);    // set back to left
        setCubeSideColor(3, cur_colors[6], cur_colors[7], cur_colors[8]);    // set right to back
    } 
    }
};

class CubeList {
    // class to contain the cubes and make doing operations on all of them easier
    public:
        int num_cubes = 27;
        Cube cubes[27];
        RotationStatus rs;    // keeps track of current rotations and actually moves cubes around after rotation animation
        // cube at index x in cubes has its current physical position at index x in this array
        // should be drawn at tracked at position x in cube_cur_positions
        // if cube_cur_positions matches {0, 1, ..., 26} then the cube is solved
        int cube_cur_positions[27]; 
        int cube_solved_positions[27];  // changed when cube is rotated to keep track of what solved looks like
        int color_index = -1;

    CubeList() {
        // set up cur_position and start_position values for each cube
        for (int x = 0; x < 27; x++) {
            cubes[x].array_position = x;
            cube_cur_positions[x] = x;  
            cube_solved_positions[x] = x;
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

    void changeColorPallete() {
        color_index = (color_index + 1) % 4;
        change_color = false;
        float color_schemes[] = {
            // default scheme: very bright
            255.0,  133.0,  2.0,    // back = orange
            255.0,  0.0,    0.0,    // front = red
            0.0,    0.0,  255.0,    // left = blue
            0.0,    255.0,  0.0,    // right = green
            255.0,  255.0,255.0,    // bottom = white
            255.0,  255.0,  0.0,    // top = yellow

            // pastel colors
            247.0, 197.0, 111.0,
            255.0, 88.0, 69.0,
            48.0, 152.0, 255.0,
            115.0, 240.0, 157.0,
            237.0, 237.0, 237.0,
            255.0, 255.0, 128.0,

            // neon 
            225, 0, 255,    // back
            255, 0, 115,    // front
            37, 52, 54,     // left
            0, 255, 247,    // right
            225, 255, 0,    // bottom
            0, 255, 21,     // top

            // grayscale ("hard" mode)
            42.5, 42.5, 42.5,
            85, 85, 85,
            127.5, 127.5, 127.5,
            170, 170, 170,
            212.5, 212.5, 212.5,
            255, 255, 255, 
        };


        // applies the color scheme at index color_index
        for (int x = color_index * 18; x < (color_index * 18) + 18; x+= 3) {
            setAllCubeSideColor((x - (18 * color_index)) / 3, color_schemes[x], color_schemes[x + 1], color_schemes[x + 2]);
        }
    }

    void swapPositions(int index1, int index2) {
        // swaps the cube objects at indices index1 and index2 in cubes[]
        Cube temp_cube = cubes[index1];
        cubes[index1] = cubes[index2];
        cubes[index2] = temp_cube; 
    }

    void rotateSide(int cur_frame) {
        rs.is_rotating = true;
        rs.rotation_start_frame = cur_frame;
        return;
    }

    void finishSideRotation() {
        int back_transforms[] = {
            18, 19, 20, 21, 22, 23, 24, 25, 26,     // position 0
            24, 21, 18, 25, 22, 19, 26, 23, 20,     // position 1
        };
        int back_color[] = {3, 0, 0};

        int front_transforms[] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8,      // position 0
            2, 5, 8, 1, 4, 7, 0, 3, 6,      // position 1
        };
        int front_color[] = {1, 0, 0};

        int left_transforms[] = {
            0, 3, 6, 9, 12, 15, 18, 21, 24,     // position 0
            6, 15, 24, 3, 12, 21, 0, 9, 18,   // position 1
        };
        int left_color[] = {0, 1, 0};

        int right_transforms[] = {
            2, 5, 8, 11, 14, 17, 20, 23, 26,    // position 0
            20, 11, 2, 23, 14, 5, 26, 17, 8,    // position 1
        };
        int right_color[] = {0, 3, 0};

        int bottom_transforms[] = {
            6, 7, 8, 15, 16, 17, 24, 25, 26,    // position 0
            8, 17, 26, 7, 16, 25, 6, 15, 24,    // position 1
        };
        int bottom_color[] = {0, 0, 3};

        int top_transforms[] = {
            0, 1, 2, 9, 10, 11, 18, 19, 20,     // position 0
            18, 9, 0, 19, 10, 1, 20, 11, 2,     // position 1
        };
        int top_color[] = {0, 0, 1};

        int equator_transforms[] = {
            3, 4, 5, 12, 13, 14, 21, 22, 23,    // position 0
            21, 12, 3, 22, 13, 4, 23, 14, 5,    // position 1
        };
        int equator_color[] = {0, 0, 1};

        // update cubes from the original cube array using their current positions in cur_cube_positions
        // want to update all the cubes whose cube_cur_positions values are along the correct layer
        // color_rotations is formatted as {# right rotations, # down rotations, # clockwise rotations}
        int *transforms, *color_rotations;
        if (rs.rotation_side == 0) { // back
            transforms = back_transforms;
            color_rotations = back_color;
        }
        if (rs.rotation_side == 1) { // front
            transforms = front_transforms;
            color_rotations = front_color;
        }
        if (rs.rotation_side == 2) { // left
            transforms = left_transforms;
            color_rotations = left_color;
        }
        if (rs.rotation_side == 3) { // right
            transforms = right_transforms;
            color_rotations = right_color;
        }
        if (rs.rotation_side == 4) { // bottom
            transforms = bottom_transforms;
            color_rotations = bottom_color;
        }
        if (rs.rotation_side == 5) { // top
            transforms = top_transforms;
            color_rotations = top_color;
        }
        if (rs.rotation_side == 6) { // equator   
            transforms = equator_transforms;
            color_rotations = equator_color;
        }
        if (rs.rotation_side == 7) { // rotate entire cube in top rotation direction
           // do top fixes
           rs.rotation_side = 5;
           finishSideRotation();
           // do middle fixes
           rs.rotation_side = 6;
           finishSideRotation();
           // do bottom fixes
           rs.rotation_side = 12;
           finishSideRotation();
           // leave because the rest of this function doesn't need to run
           rs.rotation_side = 7;
           return;
        }
        if (rs.rotation_side == 15) { // rotate entire cube in bottom rotation direction
           // do top fixes
           rs.rotation_side = 13;
           finishSideRotation();
           // do middle fixes
           rs.rotation_side = 14;
           finishSideRotation();
           // do bottom fixes
           rs.rotation_side = 4;
           finishSideRotation();
           // leave because the rest of this function doesn't need to run
           rs.rotation_side = 15;
           return;
        }
        if (rs.rotation_side >= 8 && rs.rotation_side <= 14) {
            // do the same transform but 3 times to mimic the reverse rotation
            rs.rotation_side -= 8;
            finishSideRotation();
            finishSideRotation();
            finishSideRotation();
            rs.rotation_side += 8;
            return;
        }
        // loop through cube_cur_positions looking for ones that need to be changed
        for (int x = 0; x < 27; x++) {
            for (int transform_index = 0; transform_index < 9; transform_index++) {
                if (cube_cur_positions[x] == transforms[transform_index]) {
                    // found a match that needs to be changed
                    // cout << "x = " << x << " t_i = " << transform_index << "\n";
                    // cout << "changed " << cube_cur_positions[x] << " to " << transforms[transform_index + 9] << "\n"; 
                    cubes[x].rotateCubeColorsRight(color_rotations[0]);
                    cubes[x].rotateCubeColorsDown(color_rotations[1]);
                    cubes[x].rotateCubeColorsClockwise(color_rotations[2]);
                    cube_cur_positions[x] = transforms[transform_index + 9];
                    break;
                }
            }
        }
    }

    void addAlgorithmToQueue(int alg[], int num_moves) {
        for (int x = 0; x < num_moves; x++) {
            move_queue.push_back(alg[x]);
        }
    }

    void processInput(GLFWwindow *window, int cur_frame) {
        const float camera_speed = speed_coefficient * delta_time; // adjust accordingly

        // W is get closer, so shrink down rotation radius
        if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) && (rotation_radius > camera_speed)) 
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
        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
            cam_height -= camera_speed;


        // do moves in move queue if there are any
        if ((rs.is_rotating == false) && (move_queue.size() != 0)) {
            // do the next move in move queue
            rs.rotation_side = move_queue[0];
            rotateSide(cur_frame);

            // move up the queue
            move_queue.erase(move_queue.begin(), move_queue.begin() + 1);
        }

        // scramble cube
        if ((glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) && (rs.is_rotating == false)) {
            int x = 0;
            while (x < 20) {
                int move_num = rand() % 15;
                if (move_num != 7) {
                    move_queue.push_back(move_num);
                    x++;
                }
            }
        }

        // insert right algorithm
        if ((glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) && (rs.is_rotating == false)) {
            int alg[] = {5, 3, 13, 11, 13, 9, 5, 1};
            addAlgorithmToQueue(alg, 8);
        }

        // insert left algorithm
        if ((glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) && (rs.is_rotating == false)) {
            int alg[] = {13, 10, 5, 2, 5, 1, 13, 9};
            addAlgorithmToQueue(alg, 8);
        }

        // make yellow cross algorithm
        if ((glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) && (rs.is_rotating == false)) {
            int alg[] = {1, 5, 3, 13, 11, 9};
            addAlgorithmToQueue(alg, 6);
        }

        // make yellow top algorithm
        if ((glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) && (rs.is_rotating == false)) {
            int alg[] = {3, 5, 11, 5, 3, 5, 5, 11};
            addAlgorithmToQueue(alg, 8);
        }

        // flip front top two corners algorithm
        if ((glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) && (rs.is_rotating == false)) {
            int alg[] = {11, 1, 11, 0, 0, 3, 9, 11, 0, 0, 3, 3, 13};
            addAlgorithmToQueue(alg, 13);
        }

        // rotate center top colors clockwise
        if ((glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) && (rs.is_rotating == false)) {
            int alg[] = {1, 1, 5, 2, 11, 1, 1, 10, 3, 5, 1, 1};
            addAlgorithmToQueue(alg, 12);
        }

        // rotate center top colors counter-clockwise
        if ((glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) && (rs.is_rotating == false)) {
            int alg[] = {1, 1, 13, 2, 11, 1, 1, 10, 3, 13, 1, 1};
            addAlgorithmToQueue(alg, 12);
        }

        // R'
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 11;
            rotateSide(cur_frame);
        }
        // R
        if ((glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 3;   // right rotation
            rotateSide(cur_frame);
        }
        // L'
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 10;
            rotateSide(cur_frame);
        }
        // L
        if ((glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 2;   // left rotation
            rotateSide(cur_frame);
        }
        // F'
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 9;
            rotateSide(cur_frame);
        }
        // F
        if ((glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 1;   // front rotation
            rotateSide(cur_frame);
        }
        // B'
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 8;
            rotateSide(cur_frame);
        }
        // B
        if ((glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 0;   // back rotation
            rotateSide(cur_frame);
        }
        // D'
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 12;
            rotateSide(cur_frame);
        }
        // D
        if ((glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 4;
            rotateSide(cur_frame);
        }
        // U'
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 13;
            rotateSide(cur_frame);
        }
        // U
        if ((glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 5;   // up rotation
            rotateSide(cur_frame);
        }
        // E' (same direction as D)
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 14;
            rotateSide(cur_frame);
        }
        // E (same direction as U)
        if ((glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 6;   // equator rotation
            rotateSide(cur_frame);
        }  
        // Rotate entire cube counterclockwise
        if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 15;   // rotate entire cube
            rotateSide(cur_frame);
        } 
        // Rotate entire cube clockwise
        if ((glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) && (rs.is_rotating == false)) {
            rs.rotation_side = 7;   // rotate entire cube
            rotateSide(cur_frame);
        }    
    }

    mat4 generateRotationModelMatrix(mat4 in_model, bool is_rotating, int start_position, float rotation_angle) {
        // takes in the model for an individual cube and returns the model matrix
        // that will put it in the right spot if it's currently being rotated
        if (!is_rotating || rotation_angle == 0.0) return in_model;

        // basic idea is to translate to the center of rotation (center of side that is rotating)
        // rotate, then translate back to original cur_position
        // translate needs to be different for each block
        // descriptions are when viewed from the right side, like the R rotation
        vec3 x_center_translate_list[] = {
            vec3(0.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, -1.0f), // closest to front, top to bottom
            vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f),    // middle to camera
            vec3(0.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f)     // farthest from camera
        };
        
        vec3 z_center_translate_list[] = {
            vec3(1.0f, -1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(-1.0f, -1.0f, 0.0f), // top
            vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),    // middle
            vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f)     // bottom
        };

        vec3 y_center_translate_list[] = {
            vec3(1.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f), vec3(-1.0f, 0.0f, -1.0f), // closest to camera
            vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f),    // middle to camera
            vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), vec3(-1.0f, 0.0f, 1.0f)     // furthest from camera
        };

        vec3 center_translate = vec3(0.0f, 0.0f, 0.0f);
        if ((rs.rotation_side == 0 || rs.rotation_side == 8) && (18 <= start_position) && (start_position <= 26))  // back
            center_translate = z_center_translate_list[start_position - 18];
        if ((rs.rotation_side == 1 || rs.rotation_side == 9) && (0 <= start_position) && (start_position <= 8))  // front
            center_translate = z_center_translate_list[start_position];
        if ((rs.rotation_side == 2 || rs.rotation_side == 10) && (start_position % 3 == 0))  // left
            center_translate = x_center_translate_list[(int) (start_position / 3)];
        if ((rs.rotation_side == 3 || rs.rotation_side == 11) && (start_position % 3 == 2))  // right
            center_translate = x_center_translate_list[(int) ((start_position - 2) / 3)];
        if ((rs.rotation_side == 4 || rs.rotation_side == 7 || rs.rotation_side == 12 || rs.rotation_side == 15) && (start_position % 9 >= 6))  // bottom
           center_translate = y_center_translate_list[((start_position % 9) - 6) + (((int) start_position / 3) - 2)];
        if ((rs.rotation_side == 5 || rs.rotation_side == 7 || rs.rotation_side == 13 || rs.rotation_side == 15) && (start_position % 9 <= 2))  // top
            center_translate = y_center_translate_list[(start_position % 9) + ((int) start_position / 3)];
        if ((rs.rotation_side == 6 || rs.rotation_side == 7 || rs.rotation_side == 14 || rs.rotation_side == 15) && (start_position % 9 >= 3 && start_position % 9 <= 5))  // middle
            center_translate = y_center_translate_list[((start_position % 9) - 3) + (((int) start_position / 3) - 1)];
        

        in_model = translate(in_model, center_translate);
        // left and right rotations (rotate on the x axis)
        if (((rs.rotation_side == 2 || rs.rotation_side == 10) && (start_position % 3 == 0)) || ((rs.rotation_side == 3 || rs.rotation_side == 11) && (start_position % 3 == 2))) {
            in_model = rotate(in_model, rotation_angle, vec3(1.0, 0.0, 0.0));
            
        } 
        // front and back rotations (rotate on the z axis)
        if (((rs.rotation_side == 1 || rs.rotation_side == 9) && (0 <= start_position) && (start_position <= 8)) || ((rs.rotation_side == 0 || rs.rotation_side == 8) && (18 <= start_position) && (start_position <= 26))) {
            in_model = rotate(in_model, rotation_angle, vec3(0.0, 0.0, 1.0));
        }

        // top, equator, and bottom rotations (rotate on the y axis)
        if (((rs.rotation_side == 4 || rs.rotation_side == 12) && ((start_position % 9 >= 6))) ||   // bottom
        ((rs.rotation_side == 5 || rs.rotation_side == 13) && (start_position % 9 <= 2)) ||         // top
        ((rs.rotation_side == 6 || rs.rotation_side == 14) && (start_position % 9 >= 3 && start_position % 9 <= 5)) ||  // equator
        (((rs.rotation_side == 7) || (rs.rotation_side == 15)) && ((start_position % 9 <= 2) || (start_position % 9 >= 6) || (start_position % 9 >= 3 && start_position % 9 <= 5)))) { // rotate entire cube
            in_model = rotate(in_model, rotation_angle, vec3(0.0, 1.0, 0.0));
        }


        center_translate *= -1;
        in_model = translate(in_model, center_translate);
        return in_model;

    }

    void setUpMVPMatrices(GLuint program_id, int width, int height, vec3 model_translate, int index) {
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

    void updateStickerTexture() {
        int width, height, nrChannels;
        unsigned char *data;
        switch(change_texture) {
            case 0:     // default to default_stickered
                data = stbi_load("default_stickered.jpg", &width, &height, &nrChannels, 0);
                break;
            case 1: 
                data = data = stbi_load("default_unstickered.jpg", &width, &height, &nrChannels, 0);
                break;         
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void checkIfSolved() {
        // checks if the cube is currently solved
        solved = true;
        for (int x = 0; x < 27; x++) {
            if (cube_solved_positions[x] != cube_cur_positions[x]) {
                solved = false;
            }
        }
    }
};

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // use this function for inputs that only have one key input like changing texture
    // use processInput for continuous inputs, like camera movement
    
    // reset camera position
    if (key == GLFW_KEY_H && action == GLFW_PRESS) 
       cam_rotation_angle = 0.0f;

    // change cube texture
    if (key == GLFW_KEY_M && action == GLFW_PRESS) 
       change_texture = (change_texture + 1) % 2;

    // change color scheme
    if (key == GLFW_KEY_N && action == GLFW_PRESS && solved) {
        change_color = true;
    }
}

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

    // new process input using glfwKeyCallback to prevent multiple inputs from one key tap
    glfwSetKeyCallback(window, keyCallback);

    // set default color pallete
    cube_list.changeColorPallete();

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
        cube_list.processInput(window, num_frames);
        cube_list.updateStickerTexture();
        if (change_color) cube_list.changeColorPallete();

        bool finished_rotation = cube_list.rs.updateRotation(num_frames);
        if (finished_rotation) {
            cube_list.finishSideRotation();
        }

        // check if the cube is solved
        cube_list.checkIfSolved();

        // clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vertex_array_ID);

        // draw all of the cubes from the cube list
        for (int x = 0; x < 27; x++) {
            cube_list.cubes[x].activateCubeColors();
            cube_list.setUpMVPMatrices(program_id, window_width, window_height, cube_translates[cube_list.cube_cur_positions[x]], cube_list.cube_cur_positions[x]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // update the rotation status
        // cube_list.rotateSide(num_frames);

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

    
    // cout << "total time = " << end_time - start_time << "\n";
    // cout << "total number of frames = " << num_frames << "\n";
    glfwDestroyWindow(window);
    glfwTerminate();
}

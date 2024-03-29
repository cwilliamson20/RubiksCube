cmake_minimum_required(VERSION 2.8.12.1)

PROJECT(rubiks)

SET(OpenGL_GL_PREFERENCE LEGACY)

find_package(OpenGL REQUIRED)
find_package(glfw3  REQUIRED)
find_package(GLEW   REQUIRED)
find_package(glm   REQUIRED)

message("GL include dir: ${OPENGL_INCLUDE_DIR}")
message("GL link libraries: ${OPENGL_gl_LIBRARY}")

add_library(shader loadShader.cpp)
add_library(image stb_image.cpp)
add_executable(rubiks main.cxx)
target_link_libraries(rubiks shader image)
if(APPLE)
  target_link_libraries(rubiks ${OPENGL_gl_LIBRARY} GLEW::glew_s glfw)
else()
  target_link_libraries(rubiks ${OPENGL_gl_LIBRARY} GLEW glfw)
endif()
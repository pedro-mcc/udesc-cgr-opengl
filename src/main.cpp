// gcc main.cpp -lglfw -lGL -lGLU -lm -o particles && ./particles

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <Particle.h>


GLFWwindow* initWindow(int w, int h){
    // cerr: acts as an unbuffered standar error stream
    if(!glfwInit()) { 
        std::cerr << "WLFW init failed\n"; exit(1); 
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(w, h, "Particle System", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) { 
        std::cerr << "GLEW init failed\n"; exit(1); 
    }
    glViewport(0, 0, w, h);
    
    return window;
}


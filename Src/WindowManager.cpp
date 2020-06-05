//
//  WindowManager.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 22/05/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#include "WindowManager.hpp"
#include <iostream>

// Error callback for GLFW error handling
void error_callback(int error, const char* description){
    std::cerr << "GLFW Error: " << description << std::endl;
}

WindowManager::WindowManager(){
    //GLFW init logic
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwSetErrorCallback(error_callback);
}

void WindowManager::createWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share){
    _width = width;
    _height = height;
    //OpenGL configureview port
    _window = glfwCreateWindow(width, height, title , nullptr, nullptr);
    glfwMakeContextCurrent(_window);
    
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.
}

void WindowManager::configureOpenGL(){
    glViewport(0, 0, _width, _height);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void WindowManager::pollEvents(){
    glfwPollEvents();
}

bool WindowManager::windowShouldClose(){
    return glfwWindowShouldClose(_window);
}

void WindowManager::closeWindow(){
    
}

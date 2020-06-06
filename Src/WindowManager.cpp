//
//  WindowManager.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 22/05/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#define GLEW_STATIC
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "WindowManager.hpp"

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
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
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
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    glViewport(0, 0, width/2, height/2);
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

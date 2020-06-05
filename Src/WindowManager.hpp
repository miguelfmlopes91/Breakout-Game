//
//  WindowManager.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 22/05/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#pragma once

#define GLEW_STATIC
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class WindowManager{//TODO: add constructor with with and heightr
public:
    WindowManager();
    void createWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
    void configureOpenGL();
    bool windowShouldClose();
    void pollEvents();
    void closeWindow();
    
    GLFWwindow& getWindow(){return *_window;}
    
private:
    GLFWwindow* _window = nullptr;
    int _width;
    int _height;
};

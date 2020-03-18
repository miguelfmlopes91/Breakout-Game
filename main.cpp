//
//  main.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 28/10/2019.
//  Copyright © 2019 Miguel Lopes. All rights reserved.
//

/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#define GLEW_STATIC
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Game.hpp"
#include "ResourceManager.hpp"

// Error callback for GLFW error handling
void error_callback(int error, const char* description);
// GLFW function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const GLuint SCREEN_WIDTH = 800;
// The height of the screen
const GLuint SCREEN_HEIGHT = 600;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[]){
    
    //GLFW init logic
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwSetErrorCallback(error_callback);

    //OpenGL configureview port

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.
    glfwSetKeyCallback(window, key_callback);//catch any window bug as well
    
    
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
    
    // OpenGL configuration
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Initialize game
    Breakout.init();
    
    // DeltaTime variables
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;
    
//    // Start Game within Menu State
//    Breakout.State = GAME_ACTIVE;
    
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        
        //deltaTime = 0.001f;
        // Manage user input
        Breakout.processInput(deltaTime);
        
        // Calculate delta time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Update Game state
        Breakout.update(deltaTime);
        
        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Breakout.render();
        
        glfwSwapBuffers(window);
    }
    
    // Delete all resources as loaded using the resource manager
    ResourceManager::Clear();
    
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode){
    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            Breakout._keysArray[key] = GL_TRUE;
        else if (action == GLFW_RELEASE)
            Breakout._keysArray[key] = GL_FALSE;
    }
}

//define an error call_back
void error_callback(int error, const char* description){
    std::cerr << "GLFW Error: " << description << std::endl;
}

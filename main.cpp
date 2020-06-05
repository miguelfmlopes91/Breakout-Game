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

#include <iostream>

#include "WindowManager.hpp"
#include "Game.hpp"
#include "ResourceManager.hpp"


// GLFW function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const GLuint SCREEN_WIDTH = 800;
// The height of the screen
const GLuint SCREEN_HEIGHT = 600;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[]){
    
    WindowManager window;
    window.createWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
    

    // OpenGL configuration
    window.configureOpenGL();
    
    // Initialize game
    Breakout.init();
    
    // DeltaTime variables
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    while (!window.windowShouldClose()){
        window.pollEvents();
        
        // Manage user input
        Breakout.processInput();//TODO: move this
        
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Update Game state
        Breakout.update(deltaTime);
        
        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Breakout.render();
        
        glfwSwapBuffers(&window.getWindow());
    }
    
    // Delete all resources as loaded using the resource manager
    ResourceManager::clear();
    
    glfwTerminate();
    return 0;
}

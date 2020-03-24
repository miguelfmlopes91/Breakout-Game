//
//  GameLevel.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 22/11/2019.
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
#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GameObject.hpp"
#include "SpriteRenderer.hpp"
#include "ResourceManager.hpp"


/// GameLevel holds all Tiles as part of a Breakout level and
/// hosts functionality to Load/render levels from the harddisk.
class GameLevel{
public:

    // Constructor
    GameLevel() { }
    // Loads level from file
    void      load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);
    // Render level
    void      draw(SpriteRenderer &renderer);
    // Check if the level is completed (all non-solid tiles are destroyed)
    GLboolean isCompleted();
public:
    // Level state
    std::vector<GameObject> _bricksVector;
private:
    // Initialize level from tile data
    void      init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};

#endif

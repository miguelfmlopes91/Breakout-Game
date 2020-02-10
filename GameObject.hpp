//
//  GameObject.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 20/11/2019.
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
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.hpp"
#include "SpriteRenderer.hpp"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // Constructor(s)
    GameObject();
    GameObject(glm::vec2 pos,
               glm::vec2 size,
               Texture2D sprite,
               glm::vec3 color = glm::vec3(1.0f),
               glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    //TODO: virtual dtor
    // Draw sprite
    virtual void Draw(SpriteRenderer &renderer);
    // Object state
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    GLfloat     Rotation;
    GLboolean   IsSolid;
    GLboolean   Destroyed;
    // Render state
    Texture2D   Sprite;
};

#endif
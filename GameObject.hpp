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
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.hpp"
#include "SpriteRenderer.hpp"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject{
public:
    GameObject();
    GameObject(glm::vec2 pos,
               glm::vec2 size,
               Texture2D&& sprite,
               glm::vec3 color = glm::vec3(1.0f),
               glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    GameObject(const GameObject& obj) = default;
    
    GameObject& operator=(const GameObject& obj) = default;
    //TODO: virtual dtor
    // Draw sprite
    virtual void draw(SpriteRenderer &renderer);
    // Object state
    glm::vec2   _position;
    glm::vec2   _size;
    glm::vec2   _velocity;
    glm::vec3   _color;
    float     _rotation;
    bool   _isSolid;
    bool   _destroyed;
    // Render state
    Texture2D   _sprite;
};

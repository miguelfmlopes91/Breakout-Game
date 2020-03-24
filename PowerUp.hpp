//
//  PowerUp.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 26/02/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

#include "GameObject.hpp"


const glm::vec2 SIZE(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.0f);

///could be a Struct.
class PowerUp : public GameObject{
public:
    // Constructor
    PowerUp(std::string type, glm::vec3 color, GLfloat duration,
            glm::vec2 position, Texture2D texture)
        : GameObject(position, SIZE, texture, color, VELOCITY),
          _type(type), _duration(duration), _activated()
    { }
    
    // PowerUp State
    std::string _type;
    GLfloat     _duration;
    GLboolean   _activated;
};


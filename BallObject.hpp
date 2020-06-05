//
//  BallObject.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 11/02/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#pragma once

#include "Texture.hpp"
#include "PowerUp.hpp"

// BallObject holds the state of the Ball object inheriting
// relevant state data from GameObject. Contains some extra
// functionality specific to Breakout's ball object that
// were too specific for within GameObject alone.
class BallObject : public GameObject{
public:
    // Constructor(s)
    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D&& sprite);
    
    /// Moves the ball, keeping it constrained within the window bounds (except bottom edge);
    ///@return new position
    glm::vec2 move(float dt, GLuint window_width);
    /// Resets the ball to original state with given position and velocity
    void reset(glm::vec2 position, glm::vec2 velocity);

    /// Ball state
    float  _radius;
    bool   _stuck;
    bool   _sticky;
    bool   _passThrough;
};

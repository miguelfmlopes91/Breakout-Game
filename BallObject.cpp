//
//  BallObject.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 11/02/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#include "BallObject.hpp"

BallObject::BallObject()
    : GameObject(), _radius(12.5f), _stuck(GL_TRUE), _sticky(GL_FALSE), _passThrough(GL_FALSE)  { }

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
    :  GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity), _radius(radius), _stuck(true) { }

glm::vec2 BallObject::move(GLfloat dt, GLuint window_width){
    // If not stuck to player board
    if (!_stuck){
        // Move the ball
        _position += _velocity * dt;
        // Then check if outside window bounds and if so, reverse velocity and restore at correct position
        if (_position.x <= 0.0f){
            _velocity.x = -_velocity.x;
            _position.x = 0.0f;
        }else if (_position.x + _size.x >= window_width){
            _velocity.x = -_velocity.x;
            _position.x = window_width - _size.x;
        }
        //if it hits the top
        if (_position.y <= 0.0f){
            _velocity.y = -_velocity.y;
            _position.y = 0.0f;
        }
    }
    return _position;
}

// Resets the ball to initial Stuck Position (if ball is outside window bounds)
void BallObject::reset(glm::vec2 position, glm::vec2 velocity){
    _position = position;
    _velocity = velocity;
    _stuck = GL_TRUE;
    _sticky = GL_FALSE;
    _passThrough = GL_FALSE;
}

//
//  GameObject.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 20/11/2019.
//  Copyright © 2019 Miguel Lopes. All rights reserved.
//

/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you c an redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#include "GameObject.hpp"


GameObject::GameObject()
                        : _position(0, 0),
                        _size(1, 1),
                        _velocity(0.0f),
                        _color(1.0f),
                        _rotation(0.0f),
                        _sprite(),
                        _isSolid(false),
                        _destroyed(false) {
    
}

GameObject::GameObject(glm::vec2 pos,
                       glm::vec2 size,
                       Texture2D sprite,
                       glm::vec3 color,
                       glm::vec2 velocity)
                                            : _position(pos),
                                            _size(size),
                                            _velocity(velocity),
                                            _color(color),
                                            _rotation(0.0f),
                                            _sprite(sprite),
                                            _isSolid(false),
                                            _destroyed(false) {
    
}

void GameObject::draw(SpriteRenderer& renderer){
    renderer.drawSprite(std::move(_sprite), _position, _size, _rotation, _color);
}

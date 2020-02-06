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
                        : Position(0, 0),
                        Size(1, 1),
                        Velocity(0.0f),
                        Color(1.0f),
                        Rotation(0.0f),
                        Sprite(),
                        IsSolid(false),
                        Destroyed(false) {
    
}

GameObject::GameObject(glm::vec2 pos,
                       glm::vec2 size,
                       Texture2D sprite,
                       glm::vec3 color,
                       glm::vec2 velocity)
                                            : Position(pos),
                                            Size(size),
                                            Velocity(velocity),
                                            Color(color),
                                            Rotation(0.0f),
                                            Sprite(sprite),
                                            IsSolid(false),
                                            Destroyed(false) {
    
}

void GameObject::Draw(SpriteRenderer &renderer){
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

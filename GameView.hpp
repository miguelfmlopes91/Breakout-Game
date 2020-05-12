//
//  GameView.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 12/05/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#pragma once
#include <GL/glew.h>

class GameView {
public:
    GameView() = delete;
    GameView(int width, int height);
    ~GameView();
    
    void init();
    
private:
    GLuint _width, _height;
};

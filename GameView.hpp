//
//  GameView.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 12/05/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#pragma once
#include <GL/glew.h>
#include <vector>

#include "GameObject.hpp"
#include "GameDefinitions.h"


class GameView {
public:
    GameView() = delete;
    GameView(int width, int height);
    ~GameView();
    
    void init(std::vector<std::vector<TileType>> tileBoard);
    void draw(SpriteRenderer &renderer);

public:
    // Level state
    std::vector<std::vector<GameObject>> _bricksVector;
private:
    void initLevel(std::vector<std::vector<TileType>> tileBoard);
    void reloadLevel();
    GLuint _width, _height;
};

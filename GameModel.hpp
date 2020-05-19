//
//  GameModel.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 12/05/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#pragma once

#include "GameLevel.hpp"
#include "GameDefinitions.h"

struct Tile{
    TileType tileType;
    bool isSolid = false;
    bool isDestroyed = false;
};

typedef  std::vector<std::vector<Tile>> TileBoard;

class GameModel {

public:
    GameModel() = default;
    
    void init();
    //Returns a vector of all loaded levels
    std::vector<TileBoard> createBoardTiles();
    // Check if the level is completed (all non-solid tiles are destroyed)
    GLboolean isCompleted();
    
    void resetLevel();
    int currentLevel(){return _currentLevel;}
    void setCurrentLevel(int level){_currentLevel = level;}
    
private:
    void loadLevels();
    
    
    
    std::vector<GameLevel>  _levelsVector;
    
    TileBoard _boardTiles;
    std::vector<TileBoard> _boardTilesLevels; //tileBoards per level
    
    GameLevel* _Level;
    
    int _width = 0;
    int _height = 0;
    int _lives = 0;
    int _currentLevel = 0;
};

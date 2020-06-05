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
#include "InputManager.hpp"

#include <memory>
#include <functional>

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};
// Represents the direction of a vector in the game
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};
//represents a game brick
struct Tile{
    TileType tileType;
    bool isSolid = false;
    bool isDestroyed = false;
};

typedef  std::vector<std::vector<Tile>> TileBoard;

//declaring the callbacks
using ToggleChaosEffect = std::function<void(bool)>;
using ToggleBallStuck   = std::function<void(bool)>;
using KeyPressed        = std::function<void(Direction)>;

class GameModel {

public:
    GameModel();
    ~GameModel() = default;
    
    void init();
    void processInput();

    // Check if the level is completed (all non-solid tiles are destroyed)
    bool isCompleted();
    
    //Returns a vector of all loaded levels
    std::vector<TileBoard> createBoardTiles();
    void resetLevel();
    int currentLevel(){return _currentLevel;}
    void setCurrentLevel(int level){_currentLevel = level;}
    
    void pushState(GameState state){state = _state;}
    GameState getState() const {return _state;}
    
    //Register listeners
    void toggleChaosEffect(ToggleChaosEffect handler);
    void toggleBallStuck(ToggleBallStuck handler);
    void setKeyPressHandler(KeyPressed handler);
private:
    void loadLevels();
    

    std::vector<GameLevel>  _levelsVector;
    TileBoard _boardTiles;
    std::vector<TileBoard> _boardTilesLevels; //tileBoards per level
    GameLevel* _Level;
    
    // Game state
    GameState _state;
    
    int _width = 0;
    int _height = 0;
    int _lives = 0;
    int _currentLevel = 0;
    bool _KeysProcessed[1024];
    
    InputManager* _inputMgr;
    
    //defining the callbacks
    ToggleChaosEffect _toggleChaosEffectCallback;
    ToggleBallStuck     _toggleBallStuck;
    KeyPressed          _keyPressCallback;
};

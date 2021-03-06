//
//  GameModel.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 12/05/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#include "GameModel.hpp"

GameModel::GameModel() : _state(GAME_MENU){
    
}

void GameModel::init(){
    loadLevels();
    _currentLevel = 0;
    _lives = 3;
    _inputMgr = new InputManager();
}

void GameModel::processInput(){
    int level = currentLevel();
    //TODO: switch case based on state
    if (_state == GAME_MENU){
        if(_inputMgr->getIsKeyDown(GLFW_KEY_ENTER) && !_inputMgr->getLastKeyDown(GLFW_KEY_ENTER)){
            _state = GAME_ACTIVE;
            _inputMgr->setLastKeyDown(GLFW_KEY_ENTER, true);
        }
        if(_inputMgr->getIsKeyDown(GLFW_KEY_W) && !_inputMgr->getLastKeyDown(GLFW_KEY_W)){
            _currentLevel = (level + 1) % 4;
            _inputMgr->setLastKeyDown(GLFW_KEY_W, true);
        }
        if(_inputMgr->getIsKeyDown(GLFW_KEY_S) && !_inputMgr->getLastKeyDown(GLFW_KEY_S)){
            if (_currentLevel > 0){
                --_currentLevel;
            }else{
                level = 3;
            }
            _inputMgr->setLastKeyDown(GLFW_KEY_S, true);
        }
        setCurrentLevel(level);
    }
    if (_state == GAME_WIN){
        if(_inputMgr->getIsKeyDown(GLFW_KEY_ENTER)){
            _inputMgr->setLastKeyDown(GLFW_KEY_ENTER, true);
            _toggleChaosEffectCallback(false);            
            _state = GAME_MENU;
        }
    }
    if (_state == GAME_ACTIVE){//TODO: remove +DT from here
                
        // Move playerboard
        if(_inputMgr->getIsKeyDown(GLFW_KEY_A)){
            _keyPressCallback(LEFT);
        }
        if(_inputMgr->getIsKeyDown(GLFW_KEY_D)){
            _keyPressCallback(RIGHT);
        }
        if(_inputMgr->getIsKeyDown(GLFW_KEY_SPACE)){
            _toggleBallStuck(false);
        }
    }
}

//TODO: refactor this, very bad. Keep a counter of living bricks and check if it's lower than 0.
bool GameModel::isCompleted(){
    auto currentBoard = _boardTilesLevels[_currentLevel];
    int height = static_cast<int>(currentBoard.size());
    int width = static_cast<int>(currentBoard[0].size()); // Note we can index vector at [0] since this function is only called if height > 0
    for (int y = 0; y < height; ++y){
        for (int x = 0; x < width; ++x){
            if (!currentBoard[y][x].isSolid && !currentBoard[y][x].isDestroyed){
                return GL_FALSE;
            }
        }
    }
    return GL_TRUE;
}

void GameModel::resetLevel(){
    //TODO: dont reload lol
    if (_currentLevel == 0)
        _levelsVector[0].load("Resources/levels/one.lvl");
    else if (_currentLevel == 1)
        _levelsVector[1].load("Resources/levels/two.lvl");
    else if (_currentLevel == 2)
        _levelsVector[2].load("Resources/levels/three.lvl");
    else if (_currentLevel == 3)
        _levelsVector[3].load("Resources/levels/four.lvl");
    //Reset lives
    _lives = 3;
}


void GameModel::loadLevels(){
    GameLevel one;
    GameLevel two;
    GameLevel three;
    GameLevel four;
    one.load("Resources/levels/one.lvl");
    two.load("Resources/levels/two.lvl");
    three.load("Resources/levels/three.lvl");
    four.load("Resources/levels/four.lvl");
    _levelsVector.push_back(one);
    _levelsVector.push_back(two);
    _levelsVector.push_back(three);
    _levelsVector.push_back(four);
}

std::vector<TileBoard> GameModel::createBoardTiles(){
    _boardTilesLevels.reserve(_levelsVector.size());
    //For each level, check its data and create an entry of Tiles
    for (int i = 0; i < _levelsVector.size(); ++i) {
        auto currentLevel = _levelsVector[i];
        int height = static_cast<int>(currentLevel._boardData.size());
        int width = static_cast<int>(currentLevel._boardData[0].size()); // Note we can index vector at [0] since this function is only called if height > 0
        
        //Now create a board of tiles
        _boardTiles.reserve(height);
        std::vector<Tile> row;
        for (int y = 0; y < height; ++y){
            row.reserve(width);
            for (int x = 0; x < width; ++x){
                //create tile based on the level file data and push it to the boardTiles
                Tile tile;
                auto type = static_cast<TileType>(currentLevel._boardData[y][x]);
                tile.tileType = type;
                
                if (type == TileType::solid) {
                    tile.isSolid = true;
                }
                row.push_back(tile);
            }
            _boardTiles.emplace_back(row);
            row.clear();
        }
        _boardTilesLevels.emplace_back(_boardTiles);
        _boardTiles.clear();
    }
    return _boardTilesLevels;
}

void GameModel::toggleChaosEffect(ToggleChaosEffect handler){
    _toggleChaosEffectCallback = handler;
}

void GameModel::toggleBallStuck(ToggleBallStuck handler){
    _toggleBallStuck = handler;
}

void GameModel::setKeyPressHandler(KeyPressed handler){
    _keyPressCallback = handler;
}

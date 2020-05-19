//
//  Game.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 28/10/2019.
//  Copyright © 2019 Miguel Lopes. All rights reserved.
//

/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ResourceManager.hpp"
#include "SpriteRenderer.hpp"
#include "GameLevel.hpp"
#include "BallObject.hpp"
#include "ParticleGenerator.hpp"
#include "PostProcessor.hpp"
#include "PowerUp.hpp"
#include "TextRenderer.hpp"

#include "GameView.hpp"
#include "GameModel.hpp"

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
// Defines a Collision typedef that represents collision data
typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game{
public:
    Game(GLuint width, GLuint height);
    ~Game();
    
    // Initialize game state (load all shaders/textures/levels)
    void init();
    // GameLoop
    void processInput(GLfloat dt);
    void update(GLfloat dt);
    void render();
    // Game state
    GLboolean               _keysArray[1024];
private:
    void doCollisions();
    void spawnPowerUps(GameObject &block);
    void updatePowerUps(GLfloat dt);
    void activatePowerUp(PowerUp &powerUp);
    void resetLevel();
    void resetPlayer();
    
    GameView* _view;
    GameModel* _model;
    // Game state
    GameState               _state;
    GLuint                  _width, _height;

    std::vector<PowerUp>    _powerUpsVector;
    GLuint                  _lives;
    GLboolean               _KeysProcessed[1024];
    // Game-related State data
    SpriteRenderer      *_renderer;
    GameObject          *_player;
    BallObject          *_ball;
    ParticleGenerator   *_particles;
    PostProcessor       *_effects;
    TextRenderer        *_text;
    //Shake animation time
    GLfloat             _shakeTime = 0.0f;
    
};

#endif

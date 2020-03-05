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
    void Init();
    // GameLoop
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
    // Reset
    void ResetLevel();
    void ResetPlayer();
    
    // Game state
    GameState               State;
    GLboolean               Keys[1024];
    GLuint                  Width, Height;
    std::vector<GameLevel>  Levels;
    GLuint                  Level;
    std::vector<PowerUp>    PowerUps;
    GLuint                  Lives;
    GLboolean               KeysProcessed[1024];

private:
    void DoCollisions();
    void SpawnPowerUps(GameObject &block);
    void UpdatePowerUps(GLfloat dt);
    void ActivatePowerUp(PowerUp &powerUp);
    
    // Game-related State data
    SpriteRenderer      *Renderer;
    GameObject          *Player;
    BallObject          *Ball;
    ParticleGenerator   *Particles;
    PostProcessor       *Effects;
    TextRenderer        *Text;
    GLfloat             ShakeTime = 0.0f;
};

#endif

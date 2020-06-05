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
#include "ParticleGenerator.hpp"
#include "PostProcessor.hpp"
#include "PowerUp.hpp"
#include "TextRenderer.hpp"
#include "BallObject.hpp"

#include "GameView.hpp"
#include "GameModel.hpp"



// Defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision;

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
    void processInput();
    void update(float dt);
    void render();
    // Game state
private:
    void doCollisions();
    void spawnPowerUps(GameObject &block);
    void updatePowerUps(float dt);
    void activatePowerUp(PowerUp &powerUp);
    void resetLevel();
    void resetPlayer();
    
    void OnChaosEffectTriggered(bool);
    void OnBallStuck(bool);
    void onKeyPressed(Direction);
    
    std::unique_ptr<GameView> _view;
    std::unique_ptr<GameModel> _model;

    GLuint                  _width, _height;

    std::vector<PowerUp>    _powerUpsVector;
    GLuint                  _lives;
    // Game-related State data
    SpriteRenderer      *_renderer;

    std::unique_ptr<GameObject> _player;
    std::unique_ptr<BallObject> _ball;
    ParticleGenerator   *_particles;
    PostProcessor       *_effects;
    TextRenderer        *_text;
    //Shake animation time
    float             _shakeTime = 0.0f;
    
};

#endif

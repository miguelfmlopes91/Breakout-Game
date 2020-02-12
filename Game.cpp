//
//  Game.cpp
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
#include "Game.hpp"
#include "ResourceManager.hpp"

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;
// AABB - AABB collision{
inline GLboolean CheckCollision(GameObject &one, GameObject &two);


Game::Game(GLuint width, GLuint height)
: State(GAME_ACTIVE), Keys(), Width(width), Height(height){
}

Game::~Game(){
    delete Renderer;
    delete Player;
}

void Game::Init(){
    // Load shaders
    ResourceManager::LoadShader("Resources/sprite.vs", "Resources/sprite.frag", nullptr, "sprite");
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // Load textures
    ResourceManager::LoadTexture("Resources/background.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture("Resources/awesomeface.png", GL_TRUE, "face");
    ResourceManager::LoadTexture("Resources/block.png", GL_FALSE, "block");
    ResourceManager::LoadTexture("Resources/block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::LoadTexture("Resources/paddle.png", true, "paddle");

    // Load levels
    GameLevel one;
    one.Load("Resources/levels/one.lvl", this->Width, this->Height * 0.5);
    GameLevel two;
    two.Load("Resources/levels/two.lvl", this->Width, this->Height * 0.5);
    GameLevel three;
    three.Load("Resources/levels/three.lvl", this->Width, this->Height * 0.5);
    GameLevel four;
    four.Load("Resources/levels/four.lvl", this->Width, this->Height * 0.5);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;    // Set render-specific controls
    
    // Configure game objects
    glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY,
        ResourceManager::GetTexture("face"));
    
    // Set render-specific controls
    Shader myShader;//TODO: FIX THIS
    myShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(myShader);
}

void Game::Update(GLfloat dt){
    Ball->Move(dt, this->Width);
    // Check for collisions
    DoCollisions();
}


void Game::ProcessInput(GLfloat dt){
    if (this->State == GAME_ACTIVE){
        GLfloat velocity = PLAYER_VELOCITY * dt;
        // Move playerboard
        if (this->Keys[GLFW_KEY_A]){
            if (Player->Position.x >= 0)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D]){
            if (Player->Position.x <= this->Width - Player->Size.x){
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        //move paddle
        if (this->Keys[GLFW_KEY_SPACE])
            Ball->Stuck = false;
    }
}

void Game::Render(){
    Texture2D myTexture;
    myTexture = ResourceManager::GetTexture("background");
    if(State == GAME_ACTIVE){
        // Draw background
        Renderer->DrawSprite(myTexture,
                             glm::vec2(0, 0),
                             glm::vec2(this->Width,
                                       this->Height),
                             0.0f);
        // Draw level
        this->Levels[this->Level].Draw(*Renderer);
        // Draw player
        Player->Draw(*Renderer);
        //Draw ball
        Ball->Draw(*Renderer);
    }
}

void Game::DoCollisions(){
    for (GameObject &box : this->Levels[this->Level].Bricks){
        if (!box.Destroyed){
            if (CheckCollision(*Ball, box)){
                if (!box.IsSolid)
                    box.Destroyed = GL_TRUE;
            }
        }
    }
}

inline GLboolean CheckCollision(GameObject &one, GameObject &two){
    // Collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // Collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // Collision only if on both axes
    return collisionX && collisionY;
}

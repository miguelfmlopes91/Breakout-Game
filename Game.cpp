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


Game::Game(GLuint width, GLuint height)
: State(GAME_ACTIVE), Keys(), Width(width), Height(height){
    
}

Game::~Game(){
    delete Renderer;
}

void Game::Init(){
    // Load shaders
    ResourceManager::LoadShader("Resources/sprite.vs", "Resources/sprite.frag", nullptr, "sprite");
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // Load textures
    // Load textures
    ResourceManager::LoadTexture("Resources/background.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture("Resources/awesomeface.png", GL_TRUE, "face");
    ResourceManager::LoadTexture("Resources/block.png", GL_FALSE, "block");
    ResourceManager::LoadTexture("Resources/block_solid.png", GL_FALSE, "block_solid");
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
    this->Level = 1;    // Set render-specific controls
    Shader myShader;
    myShader = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(myShader);
}

void Game::Update(GLfloat dt){
    
}


void Game::ProcessInput(GLfloat dt){
    
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
    }
}

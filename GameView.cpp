//
//  GameView.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 12/05/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#include "GameView.hpp"
#include "ResourceManager.hpp"


GameView::GameView(int width, int height): _width(width), _height(height){
    
}

void GameView::init(){
    // Load shaders
    ResourceManager::loadShader("Resources/shaders/sprite.vs", "Resources/shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::loadShader("Resources/shaders/particle.vs", "Resources/shaders/particle.frag", nullptr, "particle");
    ResourceManager::loadShader("Resources/shaders/post_processing.vs", "Resources/shaders/post_processing.frag", nullptr, "postprocessing");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f,static_cast<GLfloat>(_width),static_cast<GLfloat>(_height),0.0f, -1.0f, 1.0f);
    ResourceManager::getShader("sprite").use().setInteger("image", 0);
    ResourceManager::getShader("sprite").setMatrix4("projection", projection);
    ResourceManager::getShader("particle").use().setInteger("sprite", 0);
    ResourceManager::getShader("particle").setMatrix4("projection", projection);
    // Load textures
    ResourceManager::loadTexture("Resources/background.jpg", GL_FALSE, "background");
    ResourceManager::loadTexture("Resources/awesomeface.png", GL_TRUE, "face");
    ResourceManager::loadTexture("Resources/block.png", GL_FALSE, "block");
    ResourceManager::loadTexture("Resources/block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::loadTexture("Resources/paddle.png", true, "paddle");
    ResourceManager::loadTexture("Resources/particle.png", GL_TRUE, "particle");
    ResourceManager::loadTexture("Resources/PowerUps/powerup_speed.png", GL_TRUE, "powerup_speed");
    ResourceManager::loadTexture("Resources/PowerUps/powerup_sticky.png", GL_TRUE, "powerup_sticky");
    ResourceManager::loadTexture("Resources/PowerUps/powerup_increase.png", GL_TRUE, "powerup_increase");
    ResourceManager::loadTexture("Resources/PowerUps/powerup_confuse.png", GL_TRUE, "powerup_confuse");
    ResourceManager::loadTexture("Resources/PowerUps/powerup_chaos.png", GL_TRUE, "powerup_chaos");
    ResourceManager::loadTexture("Resources/PowerUps/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");
    
    
}

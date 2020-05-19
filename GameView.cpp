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

void GameView::init(std::vector<std::vector<TileType>> tileBoard){
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
    
    
    //init View board
    initLevel(tileBoard);
}

void GameView::initLevel(std::vector<std::vector<TileType>> tileBoard){
    int yTiles =  static_cast<int>(tileBoard.size());
    int xTiles =  static_cast<int>(tileBoard[0].size());
    
    float unit_width = static_cast<float>(_width/xTiles);
    float unit_height = static_cast<float>(_height*0.5/yTiles); //multiply by .5 to ocuppy half the screen
    
    _bricksVector.reserve(yTiles);
    std::vector<GameObject> row;
    // Initialize level tiles based on tileData
    for (int y = 0; y < yTiles; ++y){
        row.reserve(xTiles);
        for (int x = 0; x < xTiles; ++x){
            // Check block type from level data (2D level array)
            if (tileBoard[y][x] == TileType::solid){ // Solid
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::getTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                row.push_back(obj);
            }
            else if (tileBoard[y][x] > TileType::solid){    // Non-solid; now determine its color based on level data
                glm::vec3 color = glm::vec3(1.0f); // original: white
                if (tileBoard[y][x] == TileType::yellow)
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                else if (tileBoard[y][x] == TileType::blue)
                    color = glm::vec3(0.0f, 0.7f, 0.0f);
                else if (tileBoard[y][x] == TileType::green)
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                else if (tileBoard[y][x] == TileType::red)
                    color = glm::vec3(1.0f, 0.5f, 0.0f);
                
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                row.emplace_back(pos, size, ResourceManager::getTexture("block"), color);
            }
        }
        _bricksVector.push_back(row);
        row.clear();
    }
}

void GameView::reloadLevel(){
    // Clear old data
    _bricksVector.clear();
}

void GameView::draw(SpriteRenderer &renderer){
    //render level
    for ( auto& vec : _bricksVector ){
        for ( auto& tile : vec ){
            if (!tile._destroyed){
                tile.draw(renderer);
            }
        }
    }
}

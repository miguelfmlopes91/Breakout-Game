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

// AABB - AABB collisions detection
//Rectangle collision
inline GLboolean checkCollision(GameObject &one, GameObject &two);
// Circle collision --Specially for BallObject, returns if collides and direction and new vector
inline Collision checkCollision(BallObject &one, GameObject &two);
// Vector Direction -- Returns the Vector Direction after a collision
inline Direction vectorDirection(glm::vec2 target);
//Calculates probability of spawning and returns if it lands in that probability
inline GLboolean shouldSpawn(GLuint chance);
//It might happen that while one of the powerup effects is active, another powerup of the same type collides with the player paddle. In that case we have more than 1 powerup of that type currently active within the game's PowerUps vector. Then, whenever one of these powerups gets deactivated, we don't want to disable its effects yet since another powerup of the same type might still be active.
inline GLboolean isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);


Game::Game(GLuint width, GLuint height)
: _state(GAME_MENU), _keysArray(), _width(width), _height(height), _level(0), _lives(3){
}

Game::~Game(){
    delete _renderer;
    delete _player;
    delete _ball;
    delete _particles;
    delete _effects;
    delete _text;
}

void Game::init(){
    // Load shaders
    ResourceManager::LoadShader("Resources/shaders/sprite.vs", "Resources/shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("Resources/shaders/particle.vs", "Resources/shaders/particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("Resources/shaders/post_processing.vs", "Resources/shaders/post_processing.frag", nullptr, "postprocessing");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f,static_cast<GLfloat>(_width),static_cast<GLfloat>(_height),0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);
    // Load textures
    ResourceManager::LoadTexture("Resources/background.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture("Resources/awesomeface.png", GL_TRUE, "face");
    ResourceManager::LoadTexture("Resources/block.png", GL_FALSE, "block");
    ResourceManager::LoadTexture("Resources/block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::LoadTexture("Resources/paddle.png", true, "paddle");
    ResourceManager::LoadTexture("Resources/particle.png", GL_TRUE, "particle");
    ResourceManager::LoadTexture("Resources/PowerUps/powerup_speed.png", GL_TRUE, "powerup_speed");
    ResourceManager::LoadTexture("Resources/PowerUps/powerup_sticky.png", GL_TRUE, "powerup_sticky");
    ResourceManager::LoadTexture("Resources/PowerUps/powerup_increase.png", GL_TRUE, "powerup_increase");
    ResourceManager::LoadTexture("Resources/PowerUps/powerup_confuse.png", GL_TRUE, "powerup_confuse");
    ResourceManager::LoadTexture("Resources/PowerUps/powerup_chaos.png", GL_TRUE, "powerup_chaos");
    ResourceManager::LoadTexture("Resources/PowerUps/powerup_passthrough.png", GL_TRUE, "powerup_passthrough");
    
    // Set render-specific controls
    Shader myShader;//TODO: FIX THIS
    myShader    = ResourceManager::GetShader("sprite");
    _renderer    = new SpriteRenderer(myShader);
    _effects     = new PostProcessor(ResourceManager::GetShader("postprocessing"), _width, _height);
    _text        = new TextRenderer(_width, _height);
    _text->Load("Resources/fonts/ocraext.TTF", 24);
    //Setup Particle System
    _particles   = new  ParticleGenerator(ResourceManager::GetShader("particle"),
                                         ResourceManager::GetTexture("particle"),500);

    // Load levels
    GameLevel one;
    GameLevel two;
    GameLevel three;
    GameLevel four;
    one.Load("Resources/levels/one.lvl", _width, _height * 0.5);
    two.Load("Resources/levels/two.lvl", _width, _height * 0.5);
    three.Load("Resources/levels/three.lvl", _width, _height * 0.5);
    four.Load("Resources/levels/four.lvl", _width, _height * 0.5);
    _levelsVector.push_back(one);
    _levelsVector.push_back(two);
    _levelsVector.push_back(three);
    _levelsVector.push_back(four);
    _level = 0;
    
    // Configure game objects
    glm::vec2 playerPos = glm::vec2(_width / 2 - PLAYER_SIZE.x / 2, _height - PLAYER_SIZE.y);
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    _player  = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    _ball    = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
    
    //Effects->Shake = GL_TRUE;
    //Effects->Confuse = GL_TRUE;
    //Effects->Chaos = GL_TRUE;
}

void Game::update(GLfloat dt){
    // Update objects
    _ball->Move(dt, _width);
    // Check for collisions
    doCollisions();
    // Update particles
    _particles->Update(dt, *_ball, 2, glm::vec2(_ball->Radius / 2));
    // Update PowerUps
    updatePowerUps(dt);
    //slowly get shake time back to zero
    if (_shakeTime > 0.0f){
        _shakeTime -= dt;
        if (_shakeTime <= 0.0f)
            _effects->Shake = false;
    }
    // Check loss condition
    if (_ball->Position.y >= _height){ // Did ball reach bottom edge?
        --_lives;
        // Did the player lose all his lives? : Game over
        if (_lives == 0){
            resetLevel();
            _state = GAME_MENU;
        }
        resetPlayer();
    }
    // Check win condition
    if (_state == GAME_ACTIVE && _levelsVector[_level].IsCompleted()){
        resetLevel();
        resetPlayer();
        _effects->Chaos = GL_TRUE;
        _state = GAME_WIN;
    }
}

void Game::processInput(GLfloat dt){
    if (_state == GAME_MENU){
        if (_keysArray[GLFW_KEY_ENTER] && !_KeysProcessed[GLFW_KEY_ENTER]){
            _state = GAME_ACTIVE;
            _KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
        if (_keysArray[GLFW_KEY_W] && !_KeysProcessed[GLFW_KEY_W]){
            _level = (_level + 1) % 4;
            _KeysProcessed[GLFW_KEY_W] = GL_TRUE;
        }if (_keysArray[GLFW_KEY_S] && !_KeysProcessed[GLFW_KEY_S]){
            if (_level > 0)
                --_level;
            else
                _level = 3;
            _KeysProcessed[GLFW_KEY_S] = GL_TRUE;
        }
    }
    if (_state == GAME_WIN){
        if (_keysArray[GLFW_KEY_ENTER]){
            _KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
            _effects->Chaos = GL_FALSE;
            _state = GAME_MENU;
        }
    }
    if (_state == GAME_ACTIVE){
        GLfloat velocity = PLAYER_VELOCITY * dt;
        // Move playerboard
        if (_keysArray[GLFW_KEY_A]){
            if (_player->Position.x >= 0){
                _player->Position.x -= velocity;
                if (_ball->Stuck)
                    _ball->Position.x -= velocity;
            }
        }
        if (_keysArray[GLFW_KEY_D]){
            if (_player->Position.x <= _width - _player->Size.x)
            {
                _player->Position.x += velocity;
                if (_ball->Stuck)
                    _ball->Position.x += velocity;
            }
        }
        if (_keysArray[GLFW_KEY_SPACE])
            _ball->Stuck = GL_FALSE;
    }
}

void Game::render(){
    Texture2D myTexture;
    myTexture = ResourceManager::GetTexture("background");//TODO:move this into Init
    if (_state == GAME_ACTIVE || _state == GAME_MENU || _state == GAME_WIN){
        // Begin rendering to postprocessing quad
        _effects->BeginRender();
        
            // Draw background
            _renderer->DrawSprite(myTexture,glm::vec2(0, 0),glm::vec2(_width, _height),0.0f);
            // Draw level
            _levelsVector[_level].Draw(*_renderer);
            // Draw player
            _player->Draw(*_renderer);
            // Draw PowerUps
            for (PowerUp &powerUp : _powerUpsVector)
                if (!powerUp.Destroyed)
                    powerUp.Draw(*_renderer);
            // Draw particles
            _particles->Draw();
            // Draw ball
            _ball->Draw(*_renderer);

        
        // End rendering to postprocessing quad
        _effects->EndRender();
        // Render postprocessing quad
        _effects->Render(glfwGetTime());
        // Render text (don't include in postprocessing)
        std::string ss(std::to_string(_lives));
        _text->RenderText("Lives:" + ss, 5.0f, 5.0f, 1.0f);
    }
    if (_state == GAME_MENU){
        _text->RenderText("Press ENTER to start", 250.0f, _height / 2, 1.0f);
        _text->RenderText("Press W or S to select level", 245.0f, _height / 2 + 20.0f, 0.75f);
    }
    if (_state == GAME_WIN){
        _text->RenderText("You WON!!!", 320.0f, _height / 2 - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        _text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, _height / 2, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

void Game::resetLevel(){
    if (_level == 0)
        _levelsVector[0].Load("Resources/levels/one.lvl", _width, _height * 0.5f);
    else if (_level == 1)
        _levelsVector[1].Load("Resources/levels/two.lvl", _width, _height * 0.5f);
    else if (_level == 2)
        _levelsVector[2].Load("Resources/levels/three.lvl", _width, _height * 0.5f);
    else if (_level == 3)
        _levelsVector[3].Load("Resources/levels/four.lvl", _width, _height * 0.5f);
    //Reset lives
    _lives = 3;
}

void Game::resetPlayer(){
    // Reset player/ball stats
    _player->Size = PLAYER_SIZE;
    _player->Position = glm::vec2(_width / 2 - PLAYER_SIZE.x / 2, _height - PLAYER_SIZE.y);
    _ball->Reset(_player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
    // Also disable all active powerups
    _effects->Chaos = _effects->Confuse = GL_FALSE;
    _ball->PassThrough = _ball->Sticky = GL_FALSE;
    _player->Color = glm::vec3(1.0f);
    _ball->Color = glm::vec3(1.0f);
}

void Game::doCollisions(){
    for (GameObject &box : _levelsVector[_level].Bricks){
        if (!box.Destroyed){
            //Ball - brick collisions
            Collision collision = checkCollision(*_ball, box);
            if (std::get<0>(collision)){ // If collision is true
                // Destroy block if not solid or Passtrhough powerup not enabled
                if (!(_ball->PassThrough && !box.IsSolid)){
                    box.Destroyed = GL_TRUE;
                    spawnPowerUps(box);
                } else {   // if block is solid, enable shake effect
                    _shakeTime = 0.05f;
                    _effects->Shake = true;
                }
                // Collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT){ // Horizontal collision
                    _ball->Velocity.x = -_ball->Velocity.x; // Reverse horizontal velocity
                    // Relocate
                    GLfloat penetration = _ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT){
                        _ball->Position.x += penetration; // Move ball to right
                    } else {
                        _ball->Position.x -= penetration; // Move ball to left;
                    }
                } else { // Vertical collision
                    _ball->Velocity.y = -_ball->Velocity.y; // Reverse vertical velocity
                    // Relocate
                    GLfloat penetration = _ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP){
                        _ball->Position.y -= penetration; // Move ball back up
                    } else {
                        _ball->Position.y += penetration; // Move ball back down
                    }
                }
            }
            
            //Player - ball collisions
            Collision result = checkCollision(*_ball, *_player);
            //The further the ball hits the paddle from its center,
            //the stronger its horizontal velocity should be.
            if (!_ball->Stuck && std::get<0>(result)){
                // Check where it hit the board, and change velocity based on where it hit the board
                GLfloat centerBoard = _player->Position.x + _player->Size.x / 2;
                GLfloat distance = (_ball->Position.x + _ball->Radius) - centerBoard;
                GLfloat percentage = distance / (_player->Size.x / 2);
                // Then move accordingly
                GLfloat strength = 2.0f;
                glm::vec2 oldVelocity = _ball->Velocity;
                _ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
                //Ball->Velocity.y = -Ball->Velocity.y;
                _ball->Velocity.y = -1 * abs(_ball->Velocity.y);//hack: assume we always have a collision at the top of the paddle

                //new velocity vector is normalized and multiplied by the length of the old velocity vector.
                //This way, the strength and thus the velocity of the ball is always consistent,
                //regardless of where it hits the paddle.
                _ball->Velocity = glm::normalize(_ball->Velocity) * glm::length(oldVelocity);
                _ball->Stuck = _ball->Sticky;
            }
            
            //check if it hits Bottom Edge
            if (_ball->Position.y >= _height){ // Did ball reach bottom edge?
                resetLevel();
                resetPlayer();
            }
        }
    }
    for (PowerUp &powerUp : _powerUpsVector){
        if (!powerUp.Destroyed){
            if (powerUp.Position.y >= _height)
                powerUp.Destroyed = GL_TRUE;
            if (checkCollision(*_player, powerUp)){    // Collided with player, now activate powerup
                activatePowerUp(powerUp);
                powerUp.Destroyed = GL_TRUE;
                powerUp.Activated = GL_TRUE;//TODO: TYPO??
            }
        }
    }
}


void Game::spawnPowerUps(GameObject &block){//TODO: Review this.
    if (shouldSpawn(75)){ // 1 in 75 chance
        _powerUpsVector.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
    }
    if (shouldSpawn(75)){
        _powerUpsVector.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
    }
    if (shouldSpawn(75)){
        _powerUpsVector.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
    }
    if (shouldSpawn(75)){
        _powerUpsVector.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
    }
    if (shouldSpawn(15)){ // Negative powerups should spawn more often
        _powerUpsVector.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
    }
    if (shouldSpawn(15)){
        _powerUpsVector.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
    }
}

void Game::updatePowerUps(GLfloat dt){
    for (PowerUp &powerUp : _powerUpsVector){
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated){
            powerUp.Duration -= dt;
            if (powerUp.Duration <= 0.0f){
                // Remove powerup from list (will later be removed)
                powerUp.Activated = GL_FALSE;
                // Deactivate effects
                if (powerUp.Type == "sticky"){
                    if (!isOtherPowerUpActive(_powerUpsVector, "sticky")){// Only reset if no other PowerUp of type sticky is active
                        _ball->Sticky = GL_FALSE;
                        _player->Color = glm::vec3(1.0f);
                    }
                } else if (powerUp.Type == "pass-through"){
                    if (!isOtherPowerUpActive(_powerUpsVector, "pass-through")){// Only reset if no other PowerUp of type pass-through is active
                        _ball->PassThrough = GL_FALSE;
                        _ball->Color = glm::vec3(1.0f);
                    }
                } else if (powerUp.Type == "confuse"){
                    if (!isOtherPowerUpActive(_powerUpsVector, "confuse")){// Only reset if no other PowerUp of type confuse is active
                        _effects->Confuse = GL_FALSE;
                    }
                } else if (powerUp.Type == "chaos"){
                    if (!isOtherPowerUpActive(_powerUpsVector, "chaos")){// Only reset if no other PowerUp of type chaos is active
                        _effects->Chaos = GL_FALSE;
                    }
                }
            }
        }
    }
    _powerUpsVector.erase(std::remove_if(_powerUpsVector.begin(),
                                    _powerUpsVector.end(),
                                    [](const PowerUp &powerUp) {return powerUp.Destroyed && !powerUp.Activated;}),
                                    _powerUpsVector.end());
}

void Game::activatePowerUp(PowerUp &powerUp){
    // Initiate a powerup based type of powerup
    if (powerUp.Type == "speed"){
        _ball->Velocity *= 1.2;
    } else if (powerUp.Type == "sticky"){
        _ball->Sticky = GL_TRUE;
        _player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    } else if (powerUp.Type == "pass-through"){
        _ball->PassThrough = GL_TRUE;
        _ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    } else if (powerUp.Type == "pad-size-increase"){
        _player->Size.x += 50;
    } else if (powerUp.Type == "confuse"){
        if (!_effects->Chaos)
            _effects->Confuse = GL_TRUE; // Only activate if chaos wasn't already active
    } else if (powerUp.Type == "chaos"){
        if (!_effects->Confuse)
            _effects->Chaos = GL_TRUE;
    }
}

inline GLboolean checkCollision(GameObject &one, GameObject &two){ // AABB - Rectangle collision
    // Collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // Collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // Collision only if on both axes
    return collisionX && collisionY;
}

inline Collision checkCollision(BallObject &one, GameObject &two){ // AABB - Circle collision
    // Get center point circle first
    glm::vec2 center(one.Position + one.Radius);
    // Calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );
    // Get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // Add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // Retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    if (glm::length(difference) <= one.Radius){
        return std::make_tuple(GL_TRUE, vectorDirection(difference), difference);
    }else {
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
    }
}

// Calculates which direction a vector is facing (N,E,S or W)
inline Direction vectorDirection(glm::vec2 target){
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),    // up
        glm::vec2(1.0f, 0.0f),    // right
        glm::vec2(0.0f, -1.0f),   // down
        glm::vec2(-1.0f, 0.0f)    // left
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for (GLuint i = 0; i < 4; i++){
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max){
            max = dot_product;
            best_match = i;
        }
    }
    return static_cast<Direction>(best_match);
}

inline GLboolean shouldSpawn(GLuint chance){
    GLuint random = rand() % chance;
    return random == 0;
}

//The function simply checks for all activated powerups if there is still any powerup active of the same type
inline GLboolean isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type){//TODO: have a different structure to store this
    for (const PowerUp &powerUp : powerUps){
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return GL_TRUE;
    }
    return GL_FALSE;
}

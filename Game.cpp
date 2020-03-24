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
    
    // Set render-specific controls
    Shader myShader;//TODO: FIX THIS
    myShader    = ResourceManager::getShader("sprite");
    _renderer    = new SpriteRenderer(myShader);
    _effects     = new PostProcessor(ResourceManager::getShader("postprocessing"), _width, _height);
    _text        = new TextRenderer(_width, _height);
    _text->load("Resources/fonts/ocraext.TTF", 24);
    //Setup Particle System
    _particles   = new  ParticleGenerator(ResourceManager::getShader("particle"),
                                         ResourceManager::getTexture("particle"),500);

    // Load levels
    GameLevel one;
    GameLevel two;
    GameLevel three;
    GameLevel four;
    one.load("Resources/levels/one.lvl", _width, _height * 0.5);
    two.load("Resources/levels/two.lvl", _width, _height * 0.5);
    three.load("Resources/levels/three.lvl", _width, _height * 0.5);
    four.load("Resources/levels/four.lvl", _width, _height * 0.5);
    _levelsVector.push_back(one);
    _levelsVector.push_back(two);
    _levelsVector.push_back(three);
    _levelsVector.push_back(four);
    _level = 0;
    
    // Configure game objects
    glm::vec2 playerPos = glm::vec2(_width / 2 - PLAYER_SIZE.x / 2, _height - PLAYER_SIZE.y);
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    _player  = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::getTexture("paddle"));
    _ball    = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::getTexture("face"));
    
    //Effects->Shake = GL_TRUE;
    //Effects->Confuse = GL_TRUE;
    //Effects->Chaos = GL_TRUE;
}

void Game::update(GLfloat dt){
    // Update objects
    _ball->move(dt, _width);
    // Check for collisions
    doCollisions();
    // Update particles
    _particles->update(dt, *_ball, 2, glm::vec2(_ball->_radius / 2));
    // Update PowerUps
    updatePowerUps(dt);
    //slowly get shake time back to zero
    if (_shakeTime > 0.0f){
        _shakeTime -= dt;
        if (_shakeTime <= 0.0f)
            _effects->Shake = false;
    }
    // Check loss condition
    if (_ball->_position.y >= _height){ // Did ball reach bottom edge?
        --_lives;
        // Did the player lose all his lives? : Game over
        if (_lives == 0){
            resetLevel();
            _state = GAME_MENU;
        }
        resetPlayer();
    }
    // Check win condition
    if (_state == GAME_ACTIVE && _levelsVector[_level].isCompleted()){
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
            if (_player->_position.x >= 0){
                _player->_position.x -= velocity;
                if (_ball->_stuck)
                    _ball->_position.x -= velocity;
            }
        }
        if (_keysArray[GLFW_KEY_D]){
            if (_player->_position.x <= _width - _player->_size.x)
            {
                _player->_position.x += velocity;
                if (_ball->_stuck)
                    _ball->_position.x += velocity;
            }
        }
        if (_keysArray[GLFW_KEY_SPACE])
            _ball->_stuck = GL_FALSE;
    }
}

void Game::render(){
    Texture2D myTexture;
    myTexture = ResourceManager::getTexture("background");//TODO:move this into Init
    if (_state == GAME_ACTIVE || _state == GAME_MENU || _state == GAME_WIN){
        // Begin rendering to postprocessing quad
        _effects->beginRender();
        
        // Draw background
        _renderer->drawSprite(myTexture,glm::vec2(0, 0),glm::vec2(_width, _height),0.0f);
        // Draw level
        _levelsVector[_level].draw(*_renderer);
        // Draw player
        _player->draw(*_renderer);
        // Draw PowerUps
        for (PowerUp &powerUp : _powerUpsVector)
            if (!powerUp._destroyed)
                powerUp.draw(*_renderer);
        // Draw particles
        _particles->draw();
        // Draw ball
        _ball->draw(*_renderer);

        
        // End rendering to postprocessing quad
        _effects->endRender();
        // Render postprocessing quad
        _effects->render(glfwGetTime());
        // Render text (don't include in postprocessing)
        std::string ss(std::to_string(_lives));
        _text->renderText("Lives:" + ss, 5.0f, 5.0f, 1.0f);
    }
    if (_state == GAME_MENU){
        _text->renderText("Press ENTER to start", 250.0f, _height / 2, 1.0f);
        _text->renderText("Press W or S to select level", 245.0f, _height / 2 + 20.0f, 0.75f);
    }
    if (_state == GAME_WIN){
        _text->renderText("You WON!!!", 320.0f, _height / 2 - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        _text->renderText("Press ENTER to retry or ESC to quit", 130.0f, _height / 2, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

void Game::resetLevel(){
    if (_level == 0)
        _levelsVector[0].load("Resources/levels/one.lvl", _width, _height * 0.5f);
    else if (_level == 1)
        _levelsVector[1].load("Resources/levels/two.lvl", _width, _height * 0.5f);
    else if (_level == 2)
        _levelsVector[2].load("Resources/levels/three.lvl", _width, _height * 0.5f);
    else if (_level == 3)
        _levelsVector[3].load("Resources/levels/four.lvl", _width, _height * 0.5f);
    //Reset lives
    _lives = 3;
}

void Game::resetPlayer(){
    // Reset player/ball stats
    _player->_size = PLAYER_SIZE;
    _player->_position = glm::vec2(_width / 2 - PLAYER_SIZE.x / 2, _height - PLAYER_SIZE.y);
    _ball->reset(_player->_position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
    // Also disable all active powerups
    _effects->Chaos = _effects->Confuse = GL_FALSE;
    _ball->_passThrough = _ball->_sticky = GL_FALSE;
    _player->_color = glm::vec3(1.0f);
    _ball->_color = glm::vec3(1.0f);
}

void Game::doCollisions(){
    for (GameObject &box : _levelsVector[_level]._bricksVector){
        if (!box._destroyed){
            //Ball - brick collisions
            Collision collision = checkCollision(*_ball, box);
            if (std::get<0>(collision)){ // If collision is true
                // Destroy block if not solid or Passtrhough powerup not enabled
                if (!(_ball->_passThrough && !box._isSolid)){
                    box._destroyed = GL_TRUE;
                    spawnPowerUps(box);
                } else {   // if block is solid, enable shake effect
                    _shakeTime = 0.05f;
                    _effects->Shake = true;
                }
                // Collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT){ // Horizontal collision
                    _ball->_velocity.x = -_ball->_velocity.x; // Reverse horizontal velocity
                    // Relocate
                    GLfloat penetration = _ball->_radius - std::abs(diff_vector.x);
                    if (dir == LEFT){
                        _ball->_position.x += penetration; // Move ball to right
                    } else {
                        _ball->_position.x -= penetration; // Move ball to left;
                    }
                } else { // Vertical collision
                    _ball->_velocity.y = -_ball->_velocity.y; // Reverse vertical velocity
                    // Relocate
                    GLfloat penetration = _ball->_radius - std::abs(diff_vector.y);
                    if (dir == UP){
                        _ball->_position.y -= penetration; // Move ball back up
                    } else {
                        _ball->_position.y += penetration; // Move ball back down
                    }
                }
            }
            
            //Player - ball collisions
            Collision result = checkCollision(*_ball, *_player);
            //The further the ball hits the paddle from its center,
            //the stronger its horizontal velocity should be.
            if (!_ball->_stuck && std::get<0>(result)){
                // Check where it hit the board, and change velocity based on where it hit the board
                GLfloat centerBoard = _player->_position.x + _player->_size.x / 2;
                GLfloat distance = (_ball->_position.x + _ball->_radius) - centerBoard;
                GLfloat percentage = distance / (_player->_size.x / 2);
                // Then move accordingly
                GLfloat strength = 2.0f;
                glm::vec2 oldVelocity = _ball->_velocity;
                _ball->_velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
                //Ball->Velocity.y = -Ball->Velocity.y;
                _ball->_velocity.y = -1 * abs(_ball->_velocity.y);//hack: assume we always have a collision at the top of the paddle

                //new velocity vector is normalized and multiplied by the length of the old velocity vector.
                //This way, the strength and thus the velocity of the ball is always consistent,
                //regardless of where it hits the paddle.
                _ball->_velocity = glm::normalize(_ball->_velocity) * glm::length(oldVelocity);
                _ball->_stuck = _ball->_sticky;
            }
            
            //check if it hits Bottom Edge
            if (_ball->_position.y >= _height){ // Did ball reach bottom edge?
                resetLevel();
                resetPlayer();
            }
        }
    }
    for (PowerUp &powerUp : _powerUpsVector){
        if (!powerUp._destroyed){
            if (powerUp._position.y >= _height)
                powerUp._destroyed = GL_TRUE;
            if (checkCollision(*_player, powerUp)){    // Collided with player, now activate powerup
                activatePowerUp(powerUp);
                powerUp._destroyed = GL_TRUE;
                powerUp._activated = GL_TRUE;//TODO: TYPO??
            }
        }
    }
}

void Game::spawnPowerUps(GameObject &block){//TODO: Review this.
    if (shouldSpawn(75)){ // 1 in 75 chance
        _powerUpsVector.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block._position, ResourceManager::getTexture("powerup_speed")));
    }
    if (shouldSpawn(75)){
        _powerUpsVector.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block._position, ResourceManager::getTexture("powerup_sticky")));
    }
    if (shouldSpawn(75)){
        _powerUpsVector.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block._position, ResourceManager::getTexture("powerup_passthrough")));
    }
    if (shouldSpawn(75)){
        _powerUpsVector.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block._position, ResourceManager::getTexture("powerup_increase")));
    }
    if (shouldSpawn(15)){ // Negative powerups should spawn more often
        _powerUpsVector.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block._position, ResourceManager::getTexture("powerup_confuse")));
    }
    if (shouldSpawn(15)){
        _powerUpsVector.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block._position, ResourceManager::getTexture("powerup_chaos")));
    }
}

void Game::updatePowerUps(GLfloat dt){
    for (PowerUp &powerUp : _powerUpsVector){
        powerUp._position += powerUp._velocity * dt;
        if (powerUp._activated){
            powerUp._duration -= dt;
            if (powerUp._duration <= 0.0f){
                // Remove powerup from list (will later be removed)
                powerUp._activated = GL_FALSE;
                // Deactivate effects
                if (powerUp._type == "sticky"){
                    if (!isOtherPowerUpActive(_powerUpsVector, "sticky")){// Only reset if no other PowerUp of type sticky is active
                        _ball->_sticky = GL_FALSE;
                        _player->_color = glm::vec3(1.0f);
                    }
                } else if (powerUp._type == "pass-through"){
                    if (!isOtherPowerUpActive(_powerUpsVector, "pass-through")){// Only reset if no other PowerUp of type pass-through is active
                        _ball->_passThrough = GL_FALSE;
                        _ball->_color = glm::vec3(1.0f);
                    }
                } else if (powerUp._type == "confuse"){
                    if (!isOtherPowerUpActive(_powerUpsVector, "confuse")){// Only reset if no other PowerUp of type confuse is active
                        _effects->Confuse = GL_FALSE;
                    }
                } else if (powerUp._type == "chaos"){
                    if (!isOtherPowerUpActive(_powerUpsVector, "chaos")){// Only reset if no other PowerUp of type chaos is active
                        _effects->Chaos = GL_FALSE;
                    }
                }
            }
        }
    }
    _powerUpsVector.erase(std::remove_if(_powerUpsVector.begin(),
                                    _powerUpsVector.end(),
                                    [](const PowerUp &powerUp) {return powerUp._destroyed && !powerUp._activated;}),
                                    _powerUpsVector.end());
}

void Game::activatePowerUp(PowerUp &powerUp){
    // Initiate a powerup based type of powerup
    if (powerUp._type == "speed"){
        _ball->_velocity *= 1.2;
    } else if (powerUp._type == "sticky"){
        _ball->_sticky = GL_TRUE;
        _player->_color = glm::vec3(1.0f, 0.5f, 1.0f);
    } else if (powerUp._type == "pass-through"){
        _ball->_passThrough = GL_TRUE;
        _ball->_color = glm::vec3(1.0f, 0.5f, 0.5f);
    } else if (powerUp._type == "pad-size-increase"){
        _player->_size.x += 50;
    } else if (powerUp._type == "confuse"){
        if (!_effects->Chaos)
            _effects->Confuse = GL_TRUE; // Only activate if chaos wasn't already active
    } else if (powerUp._type == "chaos"){
        if (!_effects->Confuse)
            _effects->Chaos = GL_TRUE;
    }
}

inline GLboolean checkCollision(GameObject &one, GameObject &two){ // AABB - Rectangle collision
    // Collision x-axis?
    bool collisionX = one._position.x + one._size.x >= two._position.x &&
        two._position.x + two._size.x >= one._position.x;
    // Collision y-axis?
    bool collisionY = one._position.y + one._size.y >= two._position.y &&
        two._position.y + two._size.y >= one._position.y;
    // Collision only if on both axes
    return collisionX && collisionY;
}

inline Collision checkCollision(BallObject &one, GameObject &two){ // AABB - Circle collision
    // Get center point circle first
    glm::vec2 center(one._position + one._radius);
    // Calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two._size.x / 2, two._size.y / 2);
    glm::vec2 aabb_center(
        two._position.x + aabb_half_extents.x,
        two._position.y + aabb_half_extents.y
    );
    // Get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // Add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // Retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    if (glm::length(difference) <= one._radius){
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
        if (powerUp._activated)
            if (powerUp._type == type)
                return GL_TRUE;
    }
    return GL_FALSE;
}

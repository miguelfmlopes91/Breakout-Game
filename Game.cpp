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

//Shake animation time
GLfloat ShakeTime = 0.0f;

// AABB - AABB collisions detection
//Rectangle collision
inline GLboolean CheckCollision(GameObject &one, GameObject &two);
// Circle collision --Specially for BallObject, returns if collides and direction and new vector
inline Collision CheckCollision(BallObject &one, GameObject &two);
// Vector Direction -- Returns the Vector Direction after a collision
inline Direction VectorDirection(glm::vec2 target);
//Calculates probability of spawning and returns if it lands in that probability
inline GLboolean ShouldSpawn(GLuint chance);
//It might happen that while one of the powerup effects is active, another powerup of the same type collides with the player paddle. In that case we have more than 1 powerup of that type currently active within the game's PowerUps vector. Then, whenever one of these powerups gets deactivated, we don't want to disable its effects yet since another powerup of the same type might still be active.
inline GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type);


Game::Game(GLuint width, GLuint height)
: State(GAME_MENU), Keys(), Width(width), Height(height), Level(0), Lives(3){
}

Game::~Game(){
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
    delete Effects;
    delete Text;
}

void Game::Init(){
    // Load shaders
    ResourceManager::LoadShader("Resources/shaders/sprite.vs", "Resources/shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("Resources/shaders/particle.vs", "Resources/shaders/particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("Resources/shaders/post_processing.vs", "Resources/shaders/post_processing.frag", nullptr, "postprocessing");

    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f,static_cast<GLfloat>(Width),static_cast<GLfloat>(Height),0.0f, -1.0f, 1.0f);
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
    Renderer    = new SpriteRenderer(myShader);
    Effects     = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
    Text        = new TextRenderer(this->Width, this->Height);
    Text->Load("Resources/fonts/ocraext.TTF", 24);

    //Setup Particle System
    Particles   = new  ParticleGenerator(ResourceManager::GetShader("particle"),
                                         ResourceManager::GetTexture("particle"),500);

    // Load levels
    GameLevel one;
    GameLevel two;
    GameLevel three;
    GameLevel four;
    one.Load("Resources/levels/one.lvl", this->Width, this->Height * 0.5);
    two.Load("Resources/levels/two.lvl", this->Width, this->Height * 0.5);
    three.Load("Resources/levels/three.lvl", this->Width, this->Height * 0.5);
    four.Load("Resources/levels/four.lvl", this->Width, this->Height * 0.5);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;
    
    // Configure game objects
    glm::vec2 playerPos = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    Player  = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
    Ball    = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
    
    //Effects->Shake = GL_TRUE;
    //Effects->Confuse = GL_TRUE;
    //Effects->Chaos = GL_TRUE;
}

void Game::Update(GLfloat dt){
    // Update objects
    Ball->Move(dt, this->Width);
    // Check for collisions
    DoCollisions();
    // Update particles
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2));
    // Update PowerUps
    this->UpdatePowerUps(dt);
    //slowly get shake time back to zero
    if (ShakeTime > 0.0f){
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = false;
    }
    // Check loss condition
    if (Ball->Position.y >= this->Height){ // Did ball reach bottom edge?
        --this->Lives;
        // Did the player lose all his lives? : Game over
        if (this->Lives == 0){
            this->ResetLevel();
            this->State = GAME_MENU;
        }
        this->ResetPlayer();
    }
    // Check win condition
    if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted()){
        this->ResetLevel();
        this->ResetPlayer();
        Effects->Chaos = GL_TRUE;
        this->State = GAME_WIN;
    }
}

void Game::ProcessInput(GLfloat dt){
    if (this->State == GAME_MENU){
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
        }
        if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W])
        {
            this->Level = (this->Level + 1) % 4;
            this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
        }
        if (this->Keys[GLFW_KEY_S] && !this->KeysProcessed[GLFW_KEY_S])
        {
            if (this->Level > 0)
                --this->Level;
            else
                this->Level = 3;
            this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
        }
    }
    if (this->State == GAME_WIN){
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
            Effects->Chaos = GL_FALSE;
            this->State = GAME_MENU;
        }
    }
    if (this->State == GAME_ACTIVE){
        GLfloat velocity = PLAYER_VELOCITY * dt;
        // Move playerboard
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }
        if (this->Keys[GLFW_KEY_SPACE])
            Ball->Stuck = GL_FALSE;
    }
}

void Game::Render(){
    Texture2D myTexture;
    myTexture = ResourceManager::GetTexture("background");//TODO:move this into Init
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_WIN){
        // Begin rendering to postprocessing quad
        Effects->BeginRender();
        
            // Draw background
            Renderer->DrawSprite(myTexture,glm::vec2(0, 0),glm::vec2(this->Width,this->Height),0.0f);
            // Draw level
            this->Levels[this->Level].Draw(*Renderer);
            // Draw player
            Player->Draw(*Renderer);
            // Draw PowerUps
            for (PowerUp &powerUp : this->PowerUps)
                if (!powerUp.Destroyed)
                    powerUp.Draw(*Renderer);
            // Draw particles
            Particles->Draw();
            // Draw ball
            Ball->Draw(*Renderer);

        
        // End rendering to postprocessing quad
        Effects->EndRender();
        // Render postprocessing quad
        Effects->Render(glfwGetTime());
        // Render text (don't include in postprocessing)
        std::string ss(std::to_string(Lives));
        Text->RenderText("Lives:" + ss, 5.0f, 5.0f, 1.0f);
    }
    if (this->State == GAME_MENU){
        Text->RenderText("Press ENTER to start", 250.0f, this->Height / 2, 1.0f);
        Text->RenderText("Press W or S to select level", 245.0f, this->Height / 2 + 20.0f, 0.75f);
    }
    if (this->State == GAME_WIN){
        Text->RenderText("You WON!!!", 320.0f, this->Height / 2 - 20.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        Text->RenderText("Press ENTER to retry or ESC to quit", 130.0f, this->Height / 2, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

void Game::ResetLevel(){
    if (this->Level == 0)
        this->Levels[0].Load("Resources/levels/one.lvl", this->Width, this->Height * 0.5f);
    else if (this->Level == 1)
        this->Levels[1].Load("Resources/levels/two.lvl", this->Width, this->Height * 0.5f);
    else if (this->Level == 2)
        this->Levels[2].Load("Resources/levels/three.lvl", this->Width, this->Height * 0.5f);
    else if (this->Level == 3)
        this->Levels[3].Load("Resources/levels/four.lvl", this->Width, this->Height * 0.5f);
    //Reset lives
    this->Lives = 3;
}

void Game::ResetPlayer(){
    // Reset player/ball stats
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
    // Also disable all active powerups
    Effects->Chaos = Effects->Confuse = GL_FALSE;
    Ball->PassThrough = Ball->Sticky = GL_FALSE;
    Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);
}

void Game::DoCollisions(){
    for (GameObject &box : this->Levels[this->Level].Bricks){
        if (!box.Destroyed){
            //Ball - brick collisions
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)){ // If collision is true
                // Destroy block if not solid or Passtrhough powerup not enabled
                if (!(Ball->PassThrough && !box.IsSolid)){
                    box.Destroyed = GL_TRUE;
                    this->SpawnPowerUps(box);
                } else {   // if block is solid, enable shake effect
                    ShakeTime = 0.05f;
                    Effects->Shake = true;
                }
                // Collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT){ // Horizontal collision
                    Ball->Velocity.x = -Ball->Velocity.x; // Reverse horizontal velocity
                    // Relocate
                    GLfloat penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT){
                        Ball->Position.x += penetration; // Move ball to right
                    } else {
                        Ball->Position.x -= penetration; // Move ball to left;
                    }
                } else { // Vertical collision
                    Ball->Velocity.y = -Ball->Velocity.y; // Reverse vertical velocity
                    // Relocate
                    GLfloat penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP){
                        Ball->Position.y -= penetration; // Move ball back up
                    } else {
                        Ball->Position.y += penetration; // Move ball back down
                    }
                }
            }
            
            //Player - ball collisions
            Collision result = CheckCollision(*Ball, *Player);
            //The further the ball hits the paddle from its center,
            //the stronger its horizontal velocity should be.
            if (!Ball->Stuck && std::get<0>(result)){
                // Check where it hit the board, and change velocity based on where it hit the board
                GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
                GLfloat distance = (Ball->Position.x + Ball->Radius) - centerBoard;
                GLfloat percentage = distance / (Player->Size.x / 2);
                // Then move accordingly
                GLfloat strength = 2.0f;
                glm::vec2 oldVelocity = Ball->Velocity;
                Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
                //Ball->Velocity.y = -Ball->Velocity.y;
                Ball->Velocity.y = -1 * abs(Ball->Velocity.y);//hack: assume we always have a collision at the top of the paddle

                //new velocity vector is normalized and multiplied by the length of the old velocity vector.
                //This way, the strength and thus the velocity of the ball is always consistent,
                //regardless of where it hits the paddle.
                Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
                Ball->Stuck = Ball->Sticky;
            }
            
            //check if it hits Bottom Edge
            if (Ball->Position.y >= this->Height){ // Did ball reach bottom edge?
                this->ResetLevel();
                this->ResetPlayer();
            }
        }
    }
    for (PowerUp &powerUp : PowerUps){
        if (!powerUp.Destroyed){
            if (powerUp.Position.y >= this->Height)
                powerUp.Destroyed = GL_TRUE;
            if (CheckCollision(*Player, powerUp)){    // Collided with player, now activate powerup
                ActivatePowerUp(powerUp);
                powerUp.Destroyed = GL_TRUE;
                powerUp.Activated = GL_TRUE;
            }
        }
    }
}


void Game::SpawnPowerUps(GameObject &block){//TODO: Review this.
    if (ShouldSpawn(75)){ // 1 in 75 chance
        PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
    }
    if (ShouldSpawn(75)){
        PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
    }
    if (ShouldSpawn(75)){
        PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
    }
    if (ShouldSpawn(75)){
        PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
    }
    if (ShouldSpawn(15)){ // Negative powerups should spawn more often
        PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
    }
    if (ShouldSpawn(15)){
        PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
    }
}

void Game::UpdatePowerUps(GLfloat dt){
    for (PowerUp &powerUp : PowerUps){
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated){
            powerUp.Duration -= dt;
            if (powerUp.Duration <= 0.0f){
                // Remove powerup from list (will later be removed)
                powerUp.Activated = GL_FALSE;
                // Deactivate effects
                if (powerUp.Type == "sticky"){
                    if (!IsOtherPowerUpActive(PowerUps, "sticky")){// Only reset if no other PowerUp of type sticky is active
                        Ball->Sticky = GL_FALSE;
                        Player->Color = glm::vec3(1.0f);
                    }
                } else if (powerUp.Type == "pass-through"){
                    if (!IsOtherPowerUpActive(PowerUps, "pass-through")){// Only reset if no other PowerUp of type pass-through is active
                        Ball->PassThrough = GL_FALSE;
                        Ball->Color = glm::vec3(1.0f);
                    }
                } else if (powerUp.Type == "confuse"){
                    if (!IsOtherPowerUpActive(PowerUps, "confuse")){// Only reset if no other PowerUp of type confuse is active
                        Effects->Confuse = GL_FALSE;
                    }
                } else if (powerUp.Type == "chaos"){
                    if (!IsOtherPowerUpActive(PowerUps, "chaos")){// Only reset if no other PowerUp of type chaos is active
                        Effects->Chaos = GL_FALSE;
                    }
                }
            }
        }
    }
    PowerUps.erase(std::remove_if(PowerUps.begin(),
                                    PowerUps.end(),
                                    [](const PowerUp &powerUp) {return powerUp.Destroyed && !powerUp.Activated;}),
                                    PowerUps.end());
}

void Game::ActivatePowerUp(PowerUp &powerUp){
    // Initiate a powerup based type of powerup
    if (powerUp.Type == "speed"){
        Ball->Velocity *= 1.2;
    } else if (powerUp.Type == "sticky"){
        Ball->Sticky = GL_TRUE;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    } else if (powerUp.Type == "pass-through"){
        Ball->PassThrough = GL_TRUE;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    } else if (powerUp.Type == "pad-size-increase"){
        Player->Size.x += 50;
    } else if (powerUp.Type == "confuse"){
        if (!Effects->Chaos)
            Effects->Confuse = GL_TRUE; // Only activate if chaos wasn't already active
    } else if (powerUp.Type == "chaos"){
        if (!Effects->Confuse)
            Effects->Chaos = GL_TRUE;
    }
}

inline GLboolean CheckCollision(GameObject &one, GameObject &two){ // AABB - Rectangle collision
    // Collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // Collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // Collision only if on both axes
    return collisionX && collisionY;
}

inline Collision CheckCollision(BallObject &one, GameObject &two){ // AABB - Circle collision
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
        return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
    }else {
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
    }
}

// Calculates which direction a vector is facing (N,E,S or W)
inline Direction VectorDirection(glm::vec2 target){
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

inline GLboolean ShouldSpawn(GLuint chance){
    GLuint random = rand() % chance;
    return random == 0;
}

//The function simply checks for all activated powerups if there is still any powerup active of the same type
inline GLboolean IsOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type){//TODO: have a different structure to store this
    for (const PowerUp &powerUp : powerUps){
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return GL_TRUE;
    }
    return GL_FALSE;
}

//
//  SpriteRenderer.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 14/11/2019.
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
#include "SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer(Shader&& shader){
    _shader = shader;
    initRenderData();
}

SpriteRenderer::~SpriteRenderer(){
    glDeleteVertexArrays(1, &_quadVAO);
}

void SpriteRenderer::drawSprite(Texture2D&& texture,
                                glm::vec2 position,
                                glm::vec2 size,
                                float rotate,
                                glm::vec3 color){
    // Prepare transformations
    _shader.use();
    glm::mat4 model = {
        
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
    
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back
    
    model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale
    
    _shader.setMatrix4("model", model);
    
    // Render textured quad
    _shader.setVector3f("spriteColor", color);
    
    glActiveTexture(GL_TEXTURE0);
    texture.bind();
    
    glBindVertexArray(_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void SpriteRenderer::initRenderData(){
    // Configure VAO/VBO
    GLuint VBO;
    float vertices[] = {
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    
    glGenVertexArrays(1, &_quadVAO);
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(_quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

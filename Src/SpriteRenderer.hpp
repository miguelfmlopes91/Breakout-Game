//
//  SpriteRenderer.hpp
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
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#define GLFW_INCLUDE_GLCOREARB

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.hpp"
#include "Shader.hpp"


class SpriteRenderer{
public:
    // Constructor (inits shaders/shapes)
    SpriteRenderer(Shader& shader) = delete;
    SpriteRenderer(Shader&& shader);
    // Destructor
    ~SpriteRenderer();
    // Renders a defined quad textured with given sprite
    void drawSprite(Texture2D&& texture, glm::vec2 position,
                    glm::vec2 size = glm::vec2(10, 10),
                    float rotate = 0.0f,
                    glm::vec3 color = glm::vec3(1.0f));
private:
    // Render state
    Shader _shader;
    GLuint _quadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};

#endif

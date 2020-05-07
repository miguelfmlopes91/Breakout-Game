//
//  PostProcessor.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 24/02/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.hpp"
#include "SpriteRenderer.hpp"
#include "Shader.hpp"


// PostProcessor hosts all PostProcessing effects for the Breakout
// Game. It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or
// Shake boolean.
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.
class PostProcessor{
public:
    PostProcessor(Shader shader, GLuint width, GLuint height);
    // Prepares the postprocessor's framebuffer operations before rendering the game
    void beginRender();
    // Should be called after rendering the game, so it stores all the rendered data into a texture object
    void endRender();
    // Renders the PostProcessor texture quad (as a screen-encompassing large sprite)
    void render(GLfloat time);
    // Options
    GLboolean Confuse, Chaos, Shake;
private:
    // Initialize quad for rendering postprocessing texture
    void initRenderData();
    // State
    Shader PostProcessingShader;
    Texture2D Texture;
    // State
    GLuint Width, Height;
    // Render state
    GLuint MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
    GLuint RBO; // RBO is used for multisampled color buffer
    GLuint VAO;
};

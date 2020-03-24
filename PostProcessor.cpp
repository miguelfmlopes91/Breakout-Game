//
//  PostProcessor.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 24/02/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#include "PostProcessor.hpp"
#include <iostream>

PostProcessor::PostProcessor(Shader shader, GLuint width, GLuint height)
    : PostProcessingShader(shader), Texture(), Width(width), Height(height), Confuse(GL_FALSE), Chaos(GL_FALSE), Shake(GL_FALSE){
    // Initialize renderbuffer/framebuffer object
    glGenFramebuffers(1, &MSFBO);
    glGenFramebuffers(1, &FBO);
    glGenRenderbuffers(1, &RBO);
        
    //Find the max GL_MAX_SAMPLES or else it will crash
    GLint max_samples;
    glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
    
    // Initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
    glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, max_samples, GL_RGB, width, height); // Allocate storage for render buffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO); // Attach MS render buffer object to framebuffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;
        
    // Also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    Texture.Generate(width, height, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.ID, 0); // Attach texture to framebuffer as its color attachment
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Initialize render data and uniforms
    initRenderData();
        PostProcessingShader.setInteger("scene", 0, GL_TRUE);
    GLfloat offset = 1.0f / 300.0f;
    GLfloat offsets[9][2] = {
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right
    };
    glUniform2fv(glGetUniformLocation(PostProcessingShader.ID, "offsets"), 9, (GLfloat*)offsets);
    GLint edge_kernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);
    GLfloat blur_kernel[9] = {
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    };
    glUniform1fv(glGetUniformLocation(PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::BeginRender(){
    glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender(){
    // Now resolve multisampled color-buffer into intermediate FBO to store to texture
    glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Binds both READ and WRITE framebuffer to default framebuffer
}

void PostProcessor::Render(GLfloat time){
    // Set uniforms/options
    PostProcessingShader.use();
    PostProcessingShader.setFloat("time", time);
    PostProcessingShader.setInteger("confuse", Confuse);
    PostProcessingShader.setInteger("chaos", Chaos);
    PostProcessingShader.setInteger("shake", Shake);
    // Render textured quad
    glActiveTexture(GL_TEXTURE0);
    Texture.Bind();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::initRenderData(){
    // Configure VAO/VBO
    GLuint VBO;
    GLfloat vertices[] = {
        // Pos        // Tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

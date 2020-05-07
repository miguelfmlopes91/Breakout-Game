//
//  TextRenderer.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 04/03/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#pragma once

#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Texture.hpp"
#include "Shader.hpp"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
    GLuint Advance;     // Horizontal offset to advance to next glyph
};

// A renderer class for rendering text displayed by a font loaded using the
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer{
public:
    TextRenderer(GLuint width, GLuint height);
    // Pre-compiles a list of characters from the given font
    void load(std::string font, GLuint fontSize);
    // Renders a string of text using the precompiled list of characters
    void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color = glm::vec3(1.0f));

private:
    // Render state
    GLuint VAO, VBO;
    // Shader used for text rendering
    Shader _textShader;
    // Holds a list of pre-compiled Characters
    std::map<GLchar, Character> _characters;
};
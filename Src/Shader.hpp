//
//  Shader.hpp
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
#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


// General purpsoe shader object. Compiles from file, generates
// compile/link-time error messages and hosts several utility
// functions for easy management.
class Shader{
public:
    // State
    GLuint ID;
    // Constructor
    Shader() { }
    // Sets the current shader as active
    Shader  &use();
    // Compiles the shader from given source code
    void    compile(const GLchar *vertexSource,
                    const GLchar *fragmentSource,
                    const GLchar *geometrySource = nullptr); // Note: geometry source code is optional
    // Utility functions
    void    setFloat    (const GLchar *name, float value, bool useShader = false);
    void    setInteger  (const GLchar *name, GLint value, bool useShader = false);
    void    setVector2f (const GLchar *name, float x, float y, bool useShader = false);
    void    setVector2f (const GLchar *name, const glm::vec2 &value, bool useShader = false);
    void    setVector3f (const GLchar *name, float x, float y, float z, bool useShader = false);
    void    setVector3f (const GLchar *name, const glm::vec3 &value, bool useShader = false);
    void    setVector4f (const GLchar *name, float x, float y, float z, float w, bool useShader = false);
    void    setVector4f (const GLchar *name, const glm::vec4 &value, bool useShader = false);
    void    setMatrix4  (const GLchar *name, const glm::mat4 &matrix, bool useShader = false);
private:
    // Checks if compilation or linking failed and if so, print the error logs
    void    checkCompileErrors(GLuint object, std::string type);
};

#endif

//
//  Shader.cpp
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
#include "Shader.hpp"

#include <iostream>

Shader &Shader::use(){
    glUseProgram(ID);
    return *this;
}

void Shader::compile(const GLchar* vertexSource,
                     const GLchar* fragmentSource,
                     const GLchar* geometrySource){
    GLuint sVertex, sFragment, gShader = 0;
    // Vertex Shader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");
    // Fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");
    // If geometry shader source code is given, also compile geometry shader
    if (geometrySource != nullptr){
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, NULL);
        glCompileShader(gShader);
        checkCompileErrors(gShader, "GEOMETRY");
    }
    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, sVertex);
    glAttachShader(ID, sFragment);
    if (geometrySource != nullptr)
        glAttachShader(ID, gShader);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource != nullptr)
        glDeleteShader(gShader);
}

void Shader::setFloat(const GLchar *name, float value, bool useShader){
    if (useShader)
        use();
    glUniform1f(glGetUniformLocation(ID, name), value);
}
void Shader::setInteger(const GLchar *name, GLint value, bool useShader){
    if (useShader)
        use();
    glUniform1i(glGetUniformLocation(ID, name), value);
}
void Shader::setVector2f(const GLchar *name, float x, float y, bool useShader){
    if (useShader)
        use();
    glUniform2f(glGetUniformLocation(ID, name), x, y);
}
void Shader::setVector2f(const GLchar *name, const glm::vec2 &value, bool useShader){
    if (useShader)
        use();
    glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
}
void Shader::setVector3f(const GLchar *name, float x, float y, float z, bool useShader){
    if (useShader)
        use();
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}
void Shader::setVector3f(const GLchar *name, const glm::vec3 &value, bool useShader){
    if (useShader)
        use();
    glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
}
void Shader::setVector4f(const GLchar *name, float x, float y, float z, float w, bool useShader){
    if (useShader)
        use();
    glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}
void Shader::setVector4f(const GLchar *name, const glm::vec4 &value, bool useShader){
    if (useShader)
        use();
    glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
}
void Shader::setMatrix4(const GLchar *name, const glm::mat4 &matrix, bool useShader){
    if (useShader)
        use();
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}


void Shader::checkCompileErrors(GLuint object, std::string type){
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM"){
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success){
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
            << infoLog << "\n -- --------------------------------------------------- -- "
            << std::endl;
        }
    }else{
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success){
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
            << infoLog << "\n -- --------------------------------------------------- -- "
            << std::endl;
        }
    }
}

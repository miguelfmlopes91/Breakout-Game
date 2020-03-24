//
//  ResourceManager.cpp
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
#include "ResourceManager.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

#include "SOIL.h"

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::_texturesMap;
std::map<std::string, Shader>       ResourceManager::_shadersMap;


Shader ResourceManager::LoadShader(const GLchar *vShaderFile,
                                   const GLchar *fShaderFile,
                                   const GLchar *gShaderFile,
                                   std::string name){
    _shadersMap[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return _shadersMap[name];
}

Shader ResourceManager::GetShader(std::string name){
    return _shadersMap[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar *file,
                                       GLboolean alpha,
                                       std::string name){
    _texturesMap[name] = loadTextureFromFile(file, alpha);
    return _texturesMap[name];
}

Texture2D ResourceManager::GetTexture(std::string name){
    return _texturesMap[name];
}

void ResourceManager::Clear(){
    // (Properly) delete all shaders
    for (auto iter : _shadersMap)
        glDeleteProgram(iter.second.ID);
    // (Properly) delete all textures
    for (auto iter : _texturesMap)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile,
                                           const GLchar *fShaderFile,
                                           const GLchar *gShaderFile){
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try {//TODO: remove this try catch.
        // Open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // If geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr) {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch (std::exception e) {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    
    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();
    const GLchar *gShaderCode = geometryCode.c_str();
    // 2. Now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar *file,
                                               GLboolean alpha){
    // Create Texture object
    Texture2D texture;
    if (alpha){
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // Load image
    int width, height;
    unsigned char* image = SOIL_load_image(file, &width, &height, 0,
                                           texture.Image_Format == GL_RGBA ?
                                           SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    // Now generate texture
    texture.Generate(width, height, image);
    // And finally free image data
    SOIL_free_image_data(image);
    return texture;
}

//
//  Texture.cpp
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
#include <iostream>

#include "Texture.hpp"


Texture2D::Texture2D()
                    :
                    Width(0),
                    Height(0),
                    Internal_Format(GL_RGB),
                    Image_Format(GL_RGB),
                    Wrap_S(GL_REPEAT),
                    Wrap_T(GL_REPEAT),
                    Filter_Min(GL_LINEAR),
                    Filter_Max(GL_LINEAR){
    glGenTextures(1, &ID);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data){
    Width = width;
    Height = height;
    // Create Texture
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, Internal_Format, width, height, 0, Image_Format, GL_UNSIGNED_BYTE, data);
    // Set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap_T);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter_Max);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, ID);
}

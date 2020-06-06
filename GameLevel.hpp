//
//  GameLevel.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 22/11/2019.
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
#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

/// GameLevel holds all Tiles as part of a Breakout level and
/// hosts functionality to Load/render levels from the harddisk.
struct GameLevel{
public:
    // Constructor
    GameLevel() = default;
    // Loads level from file
    void load(const GLchar *file){//TODO: move to imp file
        // Clear old data
        _boardData.clear();
        // Load from file
        GLuint tileCode;
        GameLevel level;
        std::string line;
        std::ifstream fstream(file);
        std::vector<std::vector<GLuint>> tileData;
        if (fstream){
            while (std::getline(fstream, line)) {// Read each line from level file
                std::istringstream sstream(line);
                std::vector<GLuint> row;
                while (sstream >> tileCode) // Read each word seperated by spaces
                    row.push_back(tileCode);
                tileData.push_back(row);
            }
            if (tileData.size() > 0){
                _boardData.reserve(tileData.size());
                _boardData.insert(_boardData.end(), std::make_move_iterator(tileData.begin()),
                std::make_move_iterator(tileData.end()));
            }else{
                std::cout<<"Error: loading level isn't available"<<std::endl;
            }
        }
    }    // Initialize level from tile data
    std::vector<std::vector<GLuint>> _boardData;
};

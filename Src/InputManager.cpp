//
//  InputManager.cpp
//  Breakout Game
//
//  Created by Miguel Lopes on 22/05/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//


#define GLEW_STATIC
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "InputManager.hpp"
#include <algorithm>

std::vector<InputManager*> InputManager::_instances;

InputManager::InputManager() : _isEnabled(true) {
  // Add this instance to the list of instances
  InputManager::_instances.push_back(this);
}

InputManager::~InputManager() {
  // Remove this instance from the list of instances
  _instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
}

bool InputManager::getIsKeyDown(int key) {
  bool result = false;
  if (_isEnabled) {
    std::map<int,bool>::iterator it = _keys.find(key);
    if (it != _keys.end()) {
      result = _keys[key];
    }
  }
  return result;
}
bool InputManager::getLastKeyDown(int key) {
  bool result = false;
  if (_isEnabled) {
    auto it = _KeyPresses.find(key);
    if (it != _KeyPresses.end()) {
      result = _KeyPresses[key].previous;
    }
  }
  return result;
}

void InputManager::setLastKeyDown(int key, bool isDown) {
  auto it = _keys.find(key);
  if (it != _keys.end()) {
    _KeyPresses[key].previous = isDown;
  }
}

void InputManager::setupKeyInputs(WindowManager& window) {
  glfwSetKeyCallback(&window.getWindow(), InputManager::callback);
}


void InputManager::callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  // Send key event to all InputManager instances
  for (InputManager* InputManager : _instances) {
      // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
          glfwSetWindowShouldClose(window, GL_TRUE);
      
      InputManager->setIsKeyDown(key, action != GLFW_RELEASE);

//      if (key >= 0 && key < 1024){
//          if (action == GLFW_PRESS)
//              Breakout._keysArray[key] = GL_TRUE;
//          else if (action == GLFW_RELEASE)
//              Breakout._keysArray[key] = GL_FALSE;
//      }
  }
}

void InputManager::setIsKeyDown(int key, bool isDown) {
    _keys[key] = isDown;
}

/*
    using Callback = std::function<void()>;

     void addBinding(int key, const Callback& callback)
     {
         m_Callbacks[key].push_back(callback);
     }

     void onKeyPress(int key)
     {
         for (Callback& callback : m_Callbacks[key])
         {
             callback();
         }
     }
 private:
     std::map<int, std::vector<Callback>> m_Callbacks;
 */

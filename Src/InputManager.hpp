//
//  InputManager.hpp
//  Breakout Game
//
//  Created by Miguel Lopes on 22/05/2020.
//  Copyright © 2020 Miguel Lopes. All rights reserved.
//

#include "WindowManager.hpp"

#include <map>
#include <vector>

struct KeyPress {
    bool current, previous;
};

class InputManager {
  // Main KeyInput functionality
  public:
    InputManager();
    ~InputManager();

    // Must be called before any KeyInput instances will work
    static void setupKeyInputs(WindowManager& window);
    
    /// If this KeyInput is enabled and the given key is monitored, returns pressed state.  Else returns false.
    bool getIsKeyDown(int key);
    /// If previous KeyInput was the given key, returns true.
    bool getLastKeyDown(int key);
    void setLastKeyDown(int key, bool isDown);
    // See _isEnabled for details
    bool getIsEnabled() { return _isEnabled; }
    void setIsEnabled(bool value) { _isEnabled = value; }
  private:
    // Used internally to update key states.  Called by the GLFW callback.
    void setIsKeyDown(int key, bool isDown);
    // Map from monitored keyes to their pressed states
    std::map<int, bool> _keys;
    std::map<int, KeyPress> _KeyPresses;
    // If disabled, KeyInput.getIsKeyDown always returns false
    bool _isEnabled;


  private:
    // The GLFW callback for key events.  Sends events to all KeyInput instances
    static void callback(
      GLFWwindow* window, int key, int scancode, int action, int mods);
    // Keep a list of all KeyInput instances and notify them all of key events
    static std::vector<InputManager*> _instances;
};

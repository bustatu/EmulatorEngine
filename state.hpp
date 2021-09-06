#ifndef STATE_H
#define STATE_H

#include "window.hpp"
#include <iostream>
#include <vector>

// Promise the manager that it's gonna get its state
class State;

// All states handler
class StateManager
{
private:
    // Stack of states
    std::vector<State*> states;

    // Running state of the app
    bool running = false;

    // Window
    Window* window;

public:
    // Init function
    void init();

    // Draw function
    void draw();

    // Update function
    void update();

    // Returns running state of the app
    bool isRunning();

    // Changes the state to another state
    void changeState(State* newState);

    // Changes the state to another state by adding it to the stack
    void pushState(State* newState);

    // Changes the state to another state by removing the current state from the stack
    void popState(); 

    // Returns window handler for window operations
    Window* getWindow();

    // Get the current state
    State* getState();

    // Quits the app
    void quit();
};

// State handler
class State
{
protected:
    // Constructor and destructors do nothing
    State() {}
    
    // Running state
    bool is_running = false;

    // Pointer to the state manager
    StateManager* stateM = nullptr;

public:
    // Get running state
    bool isRunning();

    // Init
    virtual void init() = 0;

    // Pause
    virtual void pause() = 0;
    
    // Resume
    virtual void resume() = 0;

    // Draw
    virtual void draw() = 0;

    // Update
    virtual void update(double dt) = 0;

    // Destroy
    virtual void destroy() = 0;

    // Change the state to another state
    void changeState(State* state);

    // Attach the state manager to the state
    void attachStateManager(StateManager* state);
};

#endif
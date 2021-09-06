#include "state.hpp"

bool State::isRunning()
{
    return is_running;
}

void State::changeState(State* state)
{
    stateM -> changeState(state);
}

void State::attachStateManager(StateManager* state)
{
    stateM = state;
}

bool StateManager::isRunning()
{
    return running && (states.size() > 0);
}

void StateManager::quit()
{
    running = false;

    // Close the window as the app has been closed
    window -> close();
    delete window;

    // Destroy all states
    for(auto x : states)
        x -> destroy();
}

void StateManager::init()
{
    // Create the window and set the title
    window = new Window();
    window -> setTitle("Emulator Engine");

    // Mark as running
    running = true;
}

State* StateManager::getState()
{
    return states[states.size() - 1];
}

Window* StateManager::getWindow()
{
    return window;
}

void StateManager::update()
{
    if(running)
        getState() -> update(window -> getDelta());
}

void StateManager::draw()
{
    if(running)
        getState() -> draw();
}

void StateManager::popState()
{
    // We can't remove from an empty stack
    if(states.size() == 0)
    {
        printf("\033[1;31m{E}: Attempted to pop empty state stack!\n\033[0m");
        exit(0);
    }

    states.pop_back();
}

void StateManager::pushState(State* newState)
{
    newState -> attachStateManager(this);
    newState -> init();
    states.push_back(newState);
}

void StateManager::changeState(State* newState)
{
    if(states.size() != 0)
    {
        // Delete the old state and save the new one
        states[states.size() - 1] -> destroy();
        states[states.size() - 1] = newState;
        newState -> attachStateManager(this);
        newState -> init();
    }
    else
        // Stack is empty push new state
        pushState(newState);
}
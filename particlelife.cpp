// SDL stuff
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// Regular C++ stuff
#include <iostream>
#include <thread>
#include <string>
#include <random>
#include <fstream>
#include <filesystem>
#include <bit>
#include <array>
#include <limits>
#include <map>

// Application running state
bool running = true;

// Rng stuff
std::random_device rd;
std::mt19937 eng(rd());
std::uniform_int_distribution<> distr(INT32_MIN, INT32_MAX);
int32_t rn() {
    return distr(eng);
}

int main(int argc, char *argv[]) {
    // DEVELOPER FLAGS
    double targetFPS = 180.0;
    double targetFrameTime = 1e9 / targetFPS;
    bool limitFPS = true;
    bool calculateFPS = true;

    // Initialize Window
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Window* window;
    SDL_Surface* surface;
    window = SDL_CreateWindow("Blind Factory", 640, 480, SDL_WINDOW_RESIZABLE);
    surface = SDL_LoadBMP("./assets/textures/h.bmp");
    SDL_BlitSurface(surface, nullptr, SDL_GetWindowSurface(window), nullptr);
    SDL_UpdateWindowSurface(window);
    
    // Initialize events and keyboard state
    SDL_Event event;
    const bool* keys = SDL_GetKeyboardState(nullptr);
    

    // Initialize controls
    struct MouseState{
        bool left;
        bool right;
        bool middle;
    };
    MouseState mouseState;
    struct KeyboardState {
        bool up;
        bool down;
        bool enter;
    };
    KeyboardState keyboardState;

    // Game loop
    while(running) {
        // Set up deltaTime
        Uint64 pretime = SDL_GetTicksNS();
        // Handle quit event and mouse click events
        mouseState = {false, false, false};
        keyboardState = {false, false, false};
        while (SDL_PollEvent(&event)) {
           if (event.type == SDL_EVENT_QUIT) {
               running = false;
           } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
               if (event.button.button == SDL_BUTTON_LEFT) {
                   mouseState.left = true;
               }
               if (event.button.button == SDL_BUTTON_RIGHT) {
                   mouseState.right = true;
               }
               if (event.button.button == SDL_BUTTON_MIDDLE) {
                   mouseState.middle = true;
               }
            } else if (event.type == SDL_EVENT_KEY_UP) {
                if (event.key.key == SDLK_LEFT) {
                    mouseState.left = true;
                }
                if (event.key.key == SDLK_RIGHT) {
                    mouseState.right = true;
                }
                if (event.key.key == SDLK_UP) {
                    mouseState.middle = true;
                    keyboardState.up = true;
                }
                if (event.key.key == SDLK_DOWN) {
                    mouseState.middle = true;
                    keyboardState.down = true;
                }
                if(event.key.key == SDLK_KP_ENTER || event.key.key == SDLK_RETURN) {
                    keyboardState.enter = true;
                }
            }
        }
        
        // FPS stuff
        Uint64 posttime = SDL_GetTicksNS();
        double deltatime = (posttime - pretime);
        if (deltatime < targetFrameTime && limitFPS) {
            SDL_DelayNS(targetFrameTime - deltatime);
            posttime = SDL_GetTicksNS();
            deltatime = (posttime - pretime);
        }
        if (calculateFPS) {
            double FPS = 1e9 / deltatime;
            std::cout << "FPS: " << FPS << std::endl;
        }
    }
    SDL_Quit();
    return 0;
}

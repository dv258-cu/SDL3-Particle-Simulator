#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


#define FRAMERATE 60.0

// Globalize window and renderer
SDL_Window *window      = NULL;
SDL_Renderer *renderer  = NULL;

// Basic animation rectangles
SDL_FRect *player       = NULL;

// Framerate globals
Uint64 NOW;
Uint64 LAST;
double deltaTime;

char dir = -1;

// Called on every frame
void update_frame() {
    // 1. Calculate Delta Time
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency()) * FRAMERATE;

    // 2. Clear the entire screen (The "Eraser")
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(renderer);

    // 3. Update Physics Logic
    float xvel = 1.0f;
    // Simple bounce logic (simplified for example)
    if (player->x >= 630 || player->x <= 0) { 
        
    }
    
    player->x += xvel * deltaTime;

    // 4. Draw the Player
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    SDL_RenderFillRect(renderer, player); // Use FillRect for a solid box

    // 5. Present EVERYTHING at once (The "Flip")
    SDL_RenderPresent(renderer);
}

// Called on start
void on_init() {
    NOW = SDL_GetPerformanceCounter();
    LAST = 0;
    deltaTime = 0;

    player = malloc(sizeof(SDL_FRect));
    player->x = 0;
    player->y = 240;
    player->w = 10;
    player->h = 10;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, player);
    SDL_RenderPresent(renderer);

    return;
}

int main(int argc, char* argv[]) {
    bool quit = false;
    SDL_Event event;

    // 1. Initialize SDL Subsystems
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // 2. Create a Window and a Renderer
    // This opens a 800x600 window with the title "SDL3 Window"
    if (!SDL_CreateWindowAndRenderer("SDL3 Window", 640, 480, 0, &window, &renderer)) {
        SDL_Log("Window/Renderer creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    on_init();

    // 3. Main Loop
    while (!quit) {
        // Check for events (like clicking the 'X' button)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.key.key == SDLK_ESCAPE) {
                quit = true;
            }
        }

        update_frame();
    }

    // 4. Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
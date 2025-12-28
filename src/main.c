#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


#define FRAMERATE 60.0


SDL_Window *window      = NULL;
SDL_Renderer *renderer  = NULL;

SDL_FRect *rectangle    = NULL;

Uint64 NOW;
Uint64 LAST;
double deltaTime;

void update_frame() {
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    
    rectangle->x += 0.5 * deltaTime;
    rectangle->y += 1 * deltaTime;
    SDL_RenderRect(renderer, rectangle);
    SDL_RenderPresent(renderer);

    deltaTime = (double)(((NOW - LAST) / (double)SDL_GetPerformanceFrequency())) * FRAMERATE;
    return;
}

void on_init() {
    NOW = SDL_GetPerformanceCounter();
    LAST = 0;
    deltaTime = 0;

    rectangle = malloc(sizeof(SDL_FRect));
    rectangle->x = 10;
    rectangle->y = 10;
    rectangle->w = 100;
    rectangle->h = 100;
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);

    
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
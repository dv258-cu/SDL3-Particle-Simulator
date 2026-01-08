// C standard libraries
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// SDL3
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define FRAMERATE 60.0 // Adjusted for smoother motion
#define PI 3.14159265358979

// Basic Color Defines (Hex: RRGGBBAA)
#define WHITE      0xFFFFFFFF
#define BLACK      0x000000FF

// Globalize window and sim_renderer
SDL_Window *window          = NULL;
SDL_Renderer *sim_renderer  = NULL;
SDL_Window *gui_window      = NULL;
SDL_Renderer *gui_renderer  = NULL;

typedef struct {
    SDL_FRect rect;      // Changed to a struct, not a pointer, to simplify memory
    float bounciness;
    Uint32 color;
    bool fill;
    float xvel;
    float yvel;
} SimRect;

// Framerate globals
Uint64 NOW;
Uint64 LAST;
double deltaTime;

// Particle Globals
SimRect cell; // Allocated on stack for simplicity
SimRect cell2;

void Init_SimRect(SimRect* particle, Uint16 particle_start_x, Uint16 particle_start_y, Uint8 particle_width, Uint8 particle_height, Uint32 particle_color) {
    particle->rect.x = particle_start_x;
    particle->rect.y = particle_start_y;
    particle->rect.w = particle_width;
    particle->rect.h = particle_height;
    particle->xvel   = 300.0f;
    particle->yvel   = 0.0f;

    particle->color = particle_color;
    particle->bounciness = 0.75;
    particle->fill = false;
}

void Draw_SimRect(SimRect* particle) {
    Uint8 r = (particle->color >> 24) & 0xFF;
    Uint8 g = (particle->color >> 16) & 0xFF;
    Uint8 b = (particle->color >>  8) & 0xFF;
    Uint8 a = (particle->color >>  0) & 0xFF;

    SDL_SetRenderDrawColor(sim_renderer, r, g, b, a);

    if (particle->fill) {
        SDL_RenderFillRect(sim_renderer, &particle->rect);
    } else {
        SDL_RenderRect(sim_renderer, &particle->rect);
    }
}

void Animate_SimRect(SimRect* particle) {
    // Bounce logic (Window width is 640)
    if (particle->rect.x + particle->rect.w >= 640 || particle->rect.x <= 0) {
        particle->xvel *= -1;
        // Move it slightly away from wall to prevent "stucking"
        particle->rect.x += (particle->xvel * 0.01f); 
    }

    // Standard movement
    particle->rect.x += (float)(particle->xvel * deltaTime);
    particle->rect.y += (float)(particle->yvel * deltaTime);
}

void update_frame() {
    // Calculate Delta Time
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

    // Clear Screen (Black background)
    SDL_SetRenderDrawColor(sim_renderer, 255, 255, 255, 255);
    SDL_RenderClear(sim_renderer);

    // Update and Draw
    Animate_SimRect(&cell);
    Animate_SimRect(&cell2);
    Draw_SimRect(&cell);
    Draw_SimRect(&cell2);

    SDL_RenderPresent(sim_renderer);
}

int main(int argc, char* argv[]) {
    bool quit = false;
    SDL_Event event;

    // 1. Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }

    // 2. Create Window and sim_Renderer
    if (!SDL_CreateWindowAndRenderer("SDL3 Particle Simulator", 640, 480, 0, &window, &sim_renderer)) {
        SDL_Quit();
        return 1;
    }

    if (!SDL_CreateWindowAndRenderer("SDL3 Particle Simulator GUI", 320, 480, 0, &gui_window, &gui_renderer)) {
        SDL_Quit();
        return 1;
    }

    // Initialize VSync
    SDL_SetRenderVSync(sim_renderer, 1);

    NOW = SDL_GetPerformanceCounter();
    Init_SimRect(&cell, 0, 0, 10, 10, BLACK);
    Init_SimRect(&cell2, 0, 20, 10, 10, BLACK);

    // 3. Main Loop
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) quit = true;
                if (event.key.key == SDLK_SPACE)  cell.color = (SDL_rand(0xFFFFFF) << 8) | 0xFF;
                if (event.key.key == SDLK_F)      cell.fill = !cell.fill;
                if (event.key.key == SDLK_R)      cell.color = WHITE;
            }
        }

        update_frame();
    }

    // 4. Cleanup
    SDL_DestroyRenderer(sim_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
// C standard libraries
#include <stdio.h>
#include <math.h>

// SDL3
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>


#define FRAMERATE 60.0
#define PI 3.14159265358979

// Basic Color Defines
#define RED         0xFF0000FF
#define GREEN       0x00FF00FF
#define BLUE        0x0000FFFF
#define BLACK       0x000000FF
#define WHITE       0xFFFFFFFF
#define DARK_GREY   0x333333FF
#define LIGHT_GREY  0xAAAAAAFF

// Globalize window and renderer
SDL_Window *window      = NULL;
SDL_Renderer *renderer  = NULL;

// Basic animation rectangles
SDL_FRect *player = NULL;

typedef struct {
    SDL_FRect *rect;
    Uint32 color;
    bool fill;
} SimRect;

// Framerate globals
Uint64 NOW;
Uint64 LAST;
double deltaTime;

// Particle Globals
SimRect *particle = NULL;

void Init_SimRect() {
    particle          = malloc(sizeof(SimRect));
    particle->rect    = malloc(sizeof(SDL_FRect));
    particle->rect->x = 315;
    particle->rect->y = 235;
    particle->rect->w = 100;
    particle->rect->h = 100;

    particle->color = WHITE;
    particle->fill  = true;
}



void Draw_SimRect() {
    Uint8 r = (particle->color >> 24) & 0xFF;
    Uint8 g = (particle->color >> 16) & 0xFF;
    Uint8 b = (particle->color >>  8) & 0xFF;
    Uint8 a = (particle->color >>  0) & 0xFF;

    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    if (particle->fill) {
        SDL_RenderFillRect(renderer, particle->rect);
    }
    else {
        SDL_RenderRect(renderer, particle->rect);
    }



}

int frameHolder = 0;
void Animate_SimRect(float xvel, float yvel) {
    float xpos = particle->rect->x;
    float ypos = particle->rect->y;

    bool hitsWall = xpos >= 540 || xpos <= 0;

    if (frameHolder > 10) {
        if(hitsWall) {
            xvel *= -1;
            yvel *= -1;

            frameHolder = 0;
        }
    }

    particle->rect->x += xvel * deltaTime;
    particle->rect->x += xvel * deltaTime;

    frameHolder++;
}

// Called on every frame
void update_frame() {
    // Calculate Delta Time for framerate setting
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency()) * FRAMERATE;

    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    Animate_SimRect(1, 0);
    Draw_SimRect();
    SDL_RenderPresent(renderer);
}

// Initializes timer for time between frames
void init_dt() {
    NOW = SDL_GetPerformanceCounter();
    LAST = 0;
    deltaTime = 0;
}

static int UserInput(void *ptr) {
    return 0;
}

int main(int argc, char* argv[]) {
    bool quit = false;
    SDL_Event event;

    SDL_srand(0);

    SDL_Thread *keybinds;
    int threadReturnValue;

    keybinds = SDL_CreateThread(UserInput, "UserInput", (void *)NULL);
    SDL_WaitThread(keybinds, &threadReturnValue);

    // 1. Initialize SDL Subsystems
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // 2. Create a Window and a Renderer
    // This opens a 800x600 window with the title "SDL3 Window"
    if (!SDL_CreateWindowAndRenderer("SDL3 Particle Simulator", 640, 480, 0, &window, &renderer)) {
        SDL_Log("Window/Renderer creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    init_dt();
    Init_SimRect();

    // 3. Main Loop
    while (!quit) {
        // Check for events (like clicking the 'X' button)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.key.key == SDLK_ESCAPE) {
                quit = true;
            }
            if (event.key.key == SDLK_SPACE) {
                Uint32 pscolor = SDL_rand(0x00FFFFFF);
                pscolor = (pscolor << 8) + 0xFF;

                particle->color = pscolor;
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
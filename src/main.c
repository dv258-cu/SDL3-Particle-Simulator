#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SIM_WIDTH 1920
#define SIM_HEIGHT 1080
#define PARTICLE_COUNT 20000
#define GRAVITY 5
#define BOUNCINESS 0.75
#define FLOOR 50

typedef enum {
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    MID_LEFT,
    MID_CENTER,
    MID_RIGHT
} ParticleStartLocation;

typedef struct {
    SDL_FRect rect;
    float xvel;
    float yvel;
    Uint32 color;
    bool fill;
} SimRect;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AtomicInt running; 
    SimRect* cells[PARTICLE_COUNT];
} RenderContext;

void UpdatePhysics(SimRect* p, double dt) {
    p->yvel += GRAVITY;

    p->rect.x += (float)(p->xvel * dt);
    p->rect.y += (float)(p->yvel * dt);

    // X-axis bounce
    if (p->rect.x + p->rect.w >= SIM_WIDTH || p->rect.x <= 0) {
        p->xvel *= -1 * BOUNCINESS;
        p->rect.x = SDL_clamp(p->rect.x, 0, SIM_WIDTH - p->rect.w);
    }
    // Y-axis bounce
    if (p->rect.y + p->rect.h >= SIM_HEIGHT - FLOOR || p->rect.y <= 0) {
        p->yvel *= -1 * BOUNCINESS;
        p->rect.y = SDL_clamp(p->rect.y, 0, SIM_HEIGHT - p->rect.h - FLOOR);
    }

}

int RenderingThread(void* data) {
    RenderContext* ctx = (RenderContext*)data;
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;

    // VSync is important in a separate thread to prevent 100% CPU usage
    SDL_SetRenderVSync(ctx->renderer, 1);

    while (SDL_GetAtomicInt(&ctx->running)) {
        last = now;
        now = SDL_GetPerformanceCounter();
        double deltaTime = (double)((now - last) / (double)SDL_GetPerformanceFrequency());

        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 255);
        SDL_RenderClear(ctx->renderer);

        for (Uint64 i = 0; i < PARTICLE_COUNT; i++) {
            SimRect* p = ctx->cells[i];
            
            // 1. Update
            UpdatePhysics(p, deltaTime);

            // 2. Draw
            Uint32 c = p->color;
            SDL_SetRenderDrawColor(ctx->renderer, (c>>24)&0xFF, (c>>16)&0xFF, (c>>8)&0xFF, c&0xFF);
            
            if (p->fill) SDL_RenderFillRect(ctx->renderer, &p->rect);
            else SDL_RenderRect(ctx->renderer, &p->rect);
        }

        SDL_RenderPresent(ctx->renderer);
    }
    return 0;
}

void InitParticles(SimRect p[PARTICLE_COUNT], ParticleStartLocation loc) {
    float startX = 0, startY = 0;

    switch(loc) {
        case TOP_LEFT:   startX = 10; startY = 10; break;
        case TOP_CENTER: startX = SIM_WIDTH/2; startY = 10; break;
        case TOP_RIGHT:  startX = SIM_WIDTH-20; startY = 10; break;
        case MID_CENTER: startX = SIM_WIDTH/2; startY = SIM_HEIGHT/2; break;
        default:         startX = 100; startY = 100; break;
    }

    for (Uint64 i = 0; i < PARTICLE_COUNT; i++) {
        p[i].rect = (SDL_FRect){startX, startY, 5.0f, 5.0f};
        // Give each particle a unique random velocity
        p[i].xvel = (float)((SDL_rand(1000) - 500)); 
        p[i].yvel = (float)((SDL_rand(1000) - 500));
        p[i].color = (SDL_rand(0xFFFFFF) << 8) | 0xFF;
        p[i].fill = true;
    }
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return 1;

    RenderContext ctx;
    if (!SDL_CreateWindowAndRenderer("SDL3 Particle Simulation", SIM_WIDTH, SIM_HEIGHT, SDL_WINDOW_BORDERLESS, &ctx.window, &ctx.renderer)) {
    return 1;
}

    SDL_srand(0);

    SimRect particles[PARTICLE_COUNT];
    InitParticles(particles, MID_CENTER);

    for (Uint64 i = 0; i < PARTICLE_COUNT; i++) {
        ctx.cells[i] = &particles[i];
    }

    SDL_SetAtomicInt(&ctx.running, 1);
    SDL_Thread* renderThread = SDL_CreateThread(RenderingThread, "RenderThread", &ctx);

    SDL_Event event;
    while (SDL_GetAtomicInt(&ctx.running)) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) SDL_SetAtomicInt(&ctx.running, 0);
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) SDL_SetAtomicInt(&ctx.running, 0);
                if (event.key.key == SDLK_SPACE) InitParticles(particles, TOP_CENTER);
            }
        }
    }

    SDL_WaitThread(renderThread, NULL);
    SDL_DestroyRenderer(ctx.renderer);
    SDL_DestroyWindow(ctx.window);
    SDL_Quit();
    return 0;
}
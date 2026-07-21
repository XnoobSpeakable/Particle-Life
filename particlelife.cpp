#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Uint64 last_time = 0;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960

#define NUM_POINTS 500

static SDL_FPoint particlesa[NUM_POINTS];
static SDL_FPoint pa_vel[NUM_POINTS];
static SDL_FPoint particlesb[NUM_POINTS];
static SDL_FPoint pb_vel[NUM_POINTS];

float nnfmod(float a, float b) {
    float r = fmod(a, b);
    if (r!=0 && ((r<0) != (b<0))) {
        r += b;
    }
    return r;
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    int i;

    SDL_SetAppMetadata("Particle Life", "0.1", "com.xnlk.particlelife");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Particle Life", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    /* set up the data for a bunch of points. */
    for (i = 0; i < SDL_arraysize(particlesa); i++) {
        particlesa[i].x = SDL_randf() * ((float) WINDOW_WIDTH);
        particlesa[i].y = SDL_randf() * ((float) WINDOW_HEIGHT);
        pa_vel[i] = { 0, 0 };
    }
    for (i = 0; i < SDL_arraysize(particlesb); i++) {
        particlesb[i].x = SDL_randf() * ((float) WINDOW_WIDTH);
        particlesb[i].y = SDL_randf() * ((float) WINDOW_HEIGHT);
        pb_vel[i] = { 0, 0 };
    }

    last_time = SDL_GetTicks();

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    const Uint64 now = SDL_GetTicks();
    const float elapsed = ((float) (now - last_time)) / 1000.0f;
    int i;

    //interaction stuff
    for (int a = 0; a < SDL_arraysize(particlesa); a++) {
        for (int b = 0; b < SDL_arraysize(particlesb); b++) {
            const float dx = particlesa[a].x - particlesb[b].x;
            const float dy = particlesa[a].y - particlesb[b].y;
            const float distance = std::sqrt(dx * dx + dy * dy);
            const SDL_FPoint direction = { dx / distance, dy / distance };
            if (50.0f < distance && distance < 100.0f) {
                pa_vel[a].x += direction.x * -5.0f / distance;
                pa_vel[a].y += direction.y * -5.0f / distance;
                pb_vel[b].x += -direction.x * -5.0f / distance;
                pb_vel[b].y += -direction.y * -5.0f / distance;
            }
        }
    }
    
    for (int a = 0; a < SDL_arraysize(particlesa); a++) {
        for (int b = 0; b < SDL_arraysize(particlesa); b++) {
            const float dx = particlesa[a].x - particlesa[b].x;
            const float dy = particlesa[a].y - particlesa[b].y;
            const float distance = std::sqrt(dx * dx + dy * dy);
            const SDL_FPoint direction = { dx / distance, dy / distance };
            if (50.0f < distance && distance < 100.0f) {
                pa_vel[a].x += direction.x * 2.0f / distance;
                pa_vel[a].y += direction.y * 2.0f / distance;
                pb_vel[b].x += -direction.x * 2.0f / distance;
                pb_vel[b].y += -direction.y * 2.0f / distance;
            }
        }
    }
    
    for (int a = 0; a < SDL_arraysize(particlesb); a++) {
        for (int b = 0; b < SDL_arraysize(particlesb); b++) {
            const float dx = particlesb[a].x - particlesb[b].x;
            const float dy = particlesb[a].y - particlesb[b].y;
            const float distance = std::sqrt(dx * dx + dy * dy);
            const SDL_FPoint direction = { dx / distance, dy / distance };
            if (50.0f < distance && distance < 100.0f) {
                pa_vel[a].x += direction.x * 2.0f / distance;
                pa_vel[a].y += direction.y * 2.0f / distance;
                pb_vel[b].x += -direction.x * 2.0f / distance;
                pb_vel[b].y += -direction.y * 2.0f / distance;
            }
        }
    }

    // velocity movement stuff
    for (i = 0; i < SDL_arraysize(particlesa); i++) {
        const SDL_FPoint distancea = { elapsed * pa_vel[i].x, elapsed * pa_vel[i].y };
        pa_vel[i].x *= 0.99f;
        pa_vel[i].y *= 0.99f;
        particlesa[i].x += distancea.x;
        particlesa[i].y += distancea.y;
        particlesa[i].x = nnfmod(particlesa[i].x, WINDOW_WIDTH);
        particlesa[i].y = nnfmod(particlesa[i].y, WINDOW_HEIGHT);

        const SDL_FPoint distanceb = { elapsed * pb_vel[i].x, elapsed * pb_vel[i].y };
        pb_vel[i].x *= 0.99f;
        pb_vel[i].y *= 0.99f;
        particlesb[i].x += distanceb.x;
        particlesb[i].y += distanceb.y;
        particlesb[i].x = nnfmod(particlesb[i].x, WINDOW_WIDTH);
        particlesb[i].y = nnfmod(particlesb[i].y, WINDOW_HEIGHT);
    }
        

    last_time = now;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPoints(renderer, particlesa, SDL_arraysize(particlesa));
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderPoints(renderer, particlesb, SDL_arraysize(particlesb));
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}
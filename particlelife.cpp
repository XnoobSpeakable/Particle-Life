#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cmath>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Uint64 last_time = 0;

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define NUM_POINTS 500

static SDL_FPoint particlesa[NUM_POINTS];
static float pa_vel[NUM_POINTS];
static SDL_FPoint particlesb[NUM_POINTS];
static float pb_vel[NUM_POINTS];

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
        pa_vel[i] = 0;
    }
    for (i = 0; i < SDL_arraysize(particlesb); i++) {
        particlesb[i].x = SDL_randf() * ((float) WINDOW_WIDTH);
        particlesb[i].y = SDL_randf() * ((float) WINDOW_HEIGHT);
        pb_vel[i] = 0;
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
            if (distance < 50.0f) {
                pa_vel[a] += 10.0f / distance;
                pb_vel[b] += 10.0f / distance;
            }
        }
    }

    // velocity movement stuff
    for (i = 0; i < SDL_arraysize(particlesa); i++) {
        const float distancea = elapsed * pa_vel[i];
        particlesa[i].x += distancea;
        particlesa[i].y += distancea;
        if ((particlesa[i].x >= WINDOW_WIDTH) || (particlesa[i].y >= WINDOW_HEIGHT)) {
            particlesa[i].x = std::fmod(particlesa[i].x, WINDOW_WIDTH);
            particlesa[i].y = std::fmod(particlesa[i].y, WINDOW_HEIGHT);
        }

        const float distanceb = elapsed * pb_vel[i];
        particlesb[i].x += distanceb;
        particlesb[i].y += distanceb;
        if ((particlesb[i].x >= WINDOW_WIDTH) || (particlesb[i].y >= WINDOW_HEIGHT)) {
            particlesb[i].x = std::fmod(particlesb[i].x, WINDOW_WIDTH);
            particlesb[i].y = std::fmod(particlesb[i].y, WINDOW_HEIGHT);
        }
    }
        

    last_time = now;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPoints(renderer, particlesa, SDL_arraysize(particlesa));
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderPoints(renderer, particlesb, SDL_arraysize(particlesb));
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}
#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 480

bool game_is_running=false;
SDL_Window *window=NULL;
SDL_Renderer *renderer=NULL;
int red_color_code=0;
Uint32 starttime;
Uint32 currenttime;


bool initializeWindow(void) {
    if(SDL_Init(SDL_INIT_VIDEO)<0) {
        printf("ERROR : SDL failed to initialize \nSDL error : '%s'\n", SDL_GetError());
        return false;
    }

    window=SDL_CreateWindow(
        "SDL introduction",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0);

        if(!window) {
            printf("ERROR : Failed to open window\nSDL Error: '%s'\n",SDL_GetError());
        return false;
        }
        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
        if (!renderer) {
        printf("ERROR : Failed to create renderer\nSDL Error: '%s'\n",SDL_GetError());
        return false;
        }
    return true;
    }
void process_input(void) {
     SDL_Event event;
     while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            game_is_running=false; 
            break;

        default:
            break;
        }
    }
}

void increment_variable() {
    red_color_code=red_color_code+35;

    if (red_color_code>=256) red_color_code=0;
}

void update(void) {
    currenttime=SDL_GetTicks();
    Uint32 elapsed_time=currenttime-starttime;

    if(elapsed_time>=500) {
        increment_variable();
        starttime=currenttime;
    }
}

void draw(void) {
    SDL_SetRenderDrawColor(renderer,red_color_code,20,205,255);

    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
}

void destroyWindow(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int arg,char **argv) {
    game_is_running=initializeWindow();
    starttime=SDL_GetTicks();
    while(game_is_running) {
        process_input();

        update();

        draw();
    }
    destroyWindow();
    return 0;
}

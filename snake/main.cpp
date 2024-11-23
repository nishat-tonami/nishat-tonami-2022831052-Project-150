#include <SDL2/SDL.h>
#include <stdio.h>
#include <vector>
#include <utility>

using namespace std;

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 480
#define SNAKE_BLOCK_SIZE 20

bool game_is_running = false;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

class Food {
    private: 
    int x;
    int y;
    public:
    
    Food() {
        respawn();
    }
    void respawn() {
        x=(rand()%(SCREEN_WIDTH/SNAKE_BLOCK_SIZE))*SNAKE_BLOCK_SIZE;
        y=(rand()%(SCREEN_HEIGHT/SNAKE_BLOCK_SIZE))*SNAKE_BLOCK_SIZE;
    }
    void draw() {
        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        SDL_Rect food_block={x,y,SNAKE_BLOCK_SIZE,SNAKE_BLOCK_SIZE};
        SDL_RenderFillRect(renderer,&food_block);
    }

};

class Snake
{
private:
    int snake_x = SCREEN_WIDTH / 2;
    int snake_y = SCREEN_HEIGHT / 2;
    int snake_dx = 0;
    int snake_dy = 0;
    vector<pair<int,int>> body; 
    bool food_is_eaten=false;

public:
    Snake() {}
    void update()
    {
        snake_x += snake_dx;
        snake_y += snake_dy;

        if (snake_x < 0)
            snake_x = SCREEN_WIDTH - SNAKE_BLOCK_SIZE;
        if (snake_x >= SCREEN_WIDTH)
            snake_x = 0;
        if (snake_y < 0)
            snake_y = SCREEN_HEIGHT - SNAKE_BLOCK_SIZE;
        if (snake_y >= SCREEN_HEIGHT)
            snake_y = 0;

        body.insert(body.begin(), make_pair(snake_x,snake_y));
        if(!food_is_eaten) body.pop_back();
    }
    void process_input(SDL_Event event)
    {
        switch (event.type)
        {
        case SDL_QUIT:
            game_is_running = false;
            break;
        case SDL_KEYDOWN:

            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                if (snake_dy == 0)
                {
                    snake_dx = 0;
                    snake_dy = -SNAKE_BLOCK_SIZE;
                }
                break;
            case SDLK_DOWN:
                if (snake_dy == 0)
                {
                    snake_dx = 0;
                    snake_dy = SNAKE_BLOCK_SIZE;
                }
                break;
            case SDLK_LEFT:
                if (snake_dx == 0)
                {
                    snake_dy = 0;
                    snake_dx = -SNAKE_BLOCK_SIZE;
                }
                break;
            case SDLK_RIGHT:
                if (snake_dx == 0)
                {
                    snake_dy = 0;
                    snake_dx = SNAKE_BLOCK_SIZE;
                }
                break;
            }
        }
    }
    void draw()
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for(int i=0;i<=body.size();i++) {
            SDL_Rect snake_block = {body[i].first,body[i].second, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE};
            SDL_RenderFillRect(renderer, &snake_block);
        }
    }
};

Snake snake;
Food food;

bool initializeWindow(void)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("ERROR : SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Snake Created",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0);

    if (!window)
    {
        printf("ERROR : Failed to open window\nSDL Error: '%s'\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("ERROR : Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
        return false;
    }
    return true;
}

void process_input(void)
{

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        snake.process_input(event);
        switch (event.type)
        {
        case SDL_QUIT:
            game_is_running = false;
            break;

            break;
        default:
            break;
        }
    }
}

void update(void)
{
    snake.update();
}

void draw(void)
{

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    snake.draw();
    food.draw();

    SDL_RenderPresent(renderer);
}

void destroyWindow(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char **argv)
{
    game_is_running = initializeWindow();

    while (game_is_running)
    {

        process_input();
        update();
        draw();

        SDL_Delay(100);
    }

    destroyWindow();

    return 0;
}
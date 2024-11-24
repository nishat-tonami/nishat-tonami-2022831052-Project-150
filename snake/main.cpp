#include <SDL2/SDL.h>
#include <stdio.h>
#include <vector>
#include <utility>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <algorithm>

using namespace std;

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 480
#define SNAKE_BLOCK_SIZE 20
int score = 0;

bool game_is_running = false;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;
bool is_game_over = false;
bool is_bonus_food_generated = false;
int bonus_food_milestone = 5;

class Obstacle
{
private:
    vector<pair<int, int>> positions;

public:
    Obstacle(int cnt)
    {
        generate_obstacles(cnt);
    }

    void generate_obstacles(int cnt)
    {
        positions.clear();
        for (int i = 0; i < cnt; i++)
        {
            int x = (rand() % (SCREEN_WIDTH / SNAKE_BLOCK_SIZE)) * SNAKE_BLOCK_SIZE;
            int y = (rand() % (SCREEN_HEIGHT / SNAKE_BLOCK_SIZE)) * SNAKE_BLOCK_SIZE;
            positions.push_back(make_pair(x, y));
        }
    }

    void draw()
    {
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        for (auto &pos : positions)
        {
            SDL_Rect obstacle_block = {pos.first, pos.second, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE};
            SDL_RenderFillRect(renderer, &obstacle_block);
        }
    }
    bool check_collision(int x, int y)
    {
        for (auto &pos : positions)
        {
            if (pos.first == x && pos.second == y)
                return true;
        }
        return false;
    }
    const vector<pair<int, int>> &get_position() const
    {
        return positions;
    }
};

Obstacle obstacles{8};

class BonusFood
{
private:
    int x;
    int y;

public:
    BonusFood()
    {
        reset();
    }
    void respawn()
    {
        do
        {
            x = (rand() % (SCREEN_WIDTH / SNAKE_BLOCK_SIZE)) * SNAKE_BLOCK_SIZE;
            y = (rand() % (SCREEN_HEIGHT / SNAKE_BLOCK_SIZE)) * SNAKE_BLOCK_SIZE;
        } while (obstacles.check_collision(x, y));
    }
    void draw()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_Rect food_block = {x, y, 2 * SNAKE_BLOCK_SIZE, 2 * SNAKE_BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &food_block);
    }
    int getx()
    {
        return x;
    }
    int gety()
    {
        return y;
    }

    void reset()
    {
        x = -100;
        y = -100;
    }
};

class Food
{
private:
    int x;
    int y;

public:
    Food()
    {
        respawn();
    }
    void respawn()
    {
        do
        {
            x = (rand() % (SCREEN_WIDTH / SNAKE_BLOCK_SIZE)) * SNAKE_BLOCK_SIZE;
            y = (rand() % (SCREEN_HEIGHT / SNAKE_BLOCK_SIZE)) * SNAKE_BLOCK_SIZE;
        } while (obstacles.check_collision(x, y));
    }
    void draw()
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect food_block = {x, y, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &food_block);
    }
    int getx()
    {
        return x;
    }
    int gety()
    {
        return y;
    }
};

class Snake
{
private:
    int snake_x = SCREEN_WIDTH / 2;
    int snake_y = SCREEN_HEIGHT / 2;
    int snake_dx = 0;
    int snake_dy = 0;
    vector<pair<int, int>> body;
    bool food_is_eaten = false;
    bool bonus_food_is_eaten = false;

public:
    Snake()
    {
        body.insert(body.begin(), make_pair(snake_x, snake_y));
    }
    void update()
    {
        snake_x += snake_dx;
        snake_y += snake_dy;

        body.insert(body.begin(), make_pair(snake_x, snake_y));
        if (!food_is_eaten && !bonus_food_is_eaten)
            body.pop_back();
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
        for (int i = 0; i < body.size(); i++)
        {
            SDL_Rect snake_block = {body[i].first, body[i].second, SNAKE_BLOCK_SIZE, SNAKE_BLOCK_SIZE};
            SDL_RenderFillRect(renderer, &snake_block);
        }
    }
    bool eat_food(int food_x, int food_y)
    {
        food_is_eaten = false;
        for (int i = 0; i < body.size(); i++)
        {
            if (body[i].first == food_x && body[i].second == food_y)
            {
                food_is_eaten = true;
            }
        }
        return food_is_eaten;
    }

    bool eat_bonus_food(int food_x, int food_y)
    {
        bonus_food_is_eaten = false;
        for (int i = 0; i < body.size(); i++)
        {
            if (body[i].first == food_x && body[i].second == food_y)
                bonus_food_is_eaten = true;
            if (body[i].first - SNAKE_BLOCK_SIZE == food_x && body[i].second == food_y)
                bonus_food_is_eaten = true;
            if (body[i].first == food_x && body[i].second - SNAKE_BLOCK_SIZE == food_y)
                bonus_food_is_eaten = true;
            if (body[i].first - SNAKE_BLOCK_SIZE == food_x && body[i].second - SNAKE_BLOCK_SIZE == food_y)
                bonus_food_is_eaten = true;
        }
        return bonus_food_is_eaten;
    }

    bool detect_collision()
    {
        for (int i = 0; i < body.size(); i++)
        {
            for (int j = i + 1; j < body.size(); j++)
            {
                if (body[i].first == body[j].first && body[i].second == body[j].second)
                {
                    return true;
                }
            }
        }
        if (body[0].first < 0 || body[0].second < 0)
            return true;
        if (body[0].first >= SCREEN_WIDTH || body[0].second >= SCREEN_HEIGHT)
            return true;

        if (obstacles.check_collision(body[0].first, body[0].second))
            return true;
        return false;
    }
};

Snake snake;
Food food;
BonusFood bonusFood;

bool initializeWindow(void)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("ERROR : SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
        return false;
    }
    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return false;
    }

    window = SDL_CreateWindow(
        "Snake Game",
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
    font = TTF_OpenFont("ShadeBlue-2OozX.ttf", 42);
    if (font == NULL)
    {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return false;
    }

    obstacles.generate_obstacles(8);

    return true;
}

void render_text(const char *text, int x, int y, SDL_Color color)
{
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, color);
    if (textSurface == NULL)
    {
        printf("Unable to render text surface! TTF_Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL)
    {
        printf("Unable to create texture from rendered text! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
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

int bonus_food_timer = 0;

void update(void)
{
    if (score >= bonus_food_milestone && score % 5 == 0 && score != 0 && is_bonus_food_generated == false)
    {
        bonusFood.respawn();
        is_bonus_food_generated = true;
        bonus_food_timer = SDL_GetTicks();
        bonus_food_milestone += 5;
    }

    if (is_bonus_food_generated && SDL_GetTicks() - bonus_food_timer > 5000)
    {
        bonusFood.reset();
        is_bonus_food_generated = false;
    }

    if (snake.eat_food(food.getx(), food.gety()))
    {
        food.respawn();
        score++;
    }

    if (snake.eat_bonus_food(bonusFood.getx(), bonusFood.gety()))
    {
        score += 3;
        bonusFood.reset();
        is_bonus_food_generated = false;
    }

    snake.update();

    if (snake.detect_collision())
    {
        is_game_over = true;
        return;
    }
}

string get_score_string(int x)
{
    string s;
    while (x > 0)
    {
        s.push_back(x % 10 + '0');
        x /= 10;
    }
    if (s.empty())
        s.push_back('0');
    reverse(s.begin(), s.end());
    s = "Score: " + s;
    return s;
}

void draw_score(int x, int y)
{
    string s = get_score_string(score);

    render_text(s.c_str(), x, y, {255, 255, 255, 255});
}

void game_over()
{
    render_text("Game Over!", (SCREEN_WIDTH / 2) - 20, SCREEN_HEIGHT / 2 - 35, {255, 255, 255, 255});
    draw_score(SCREEN_WIDTH / 2 - 20, SCREEN_HEIGHT / 2);
}
void draw(void)
{

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    snake.draw();
    food.draw();
    bonusFood.draw();
    obstacles.draw();

    if (is_game_over)
        game_over();
    else
        draw_score(20, 20);

    SDL_RenderPresent(renderer);
}

void destroyWindow(void)
{
    TTF_CloseFont(font);
    font = NULL;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char **argv)
{
    game_is_running = initializeWindow();

    while (game_is_running)
    {
        process_input();
        if (!is_game_over)
        {
            update();
        }

        draw();

        SDL_Delay(100);
    }

    destroyWindow();

    return 0;
}
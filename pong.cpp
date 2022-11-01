// Including
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <iostream>
#include <cmath>

// Defining
#define WIDTH 720
#define HEIGHT 720
#define FONT_SIZE 32
#define PADDLE_SPEED 10
#define BALL_SIZE 10
#define BALL_SPEED 10
#define PI 3.14159265358979323846

SDL_Renderer* renderer;
SDL_Window* window;
TTF_Font* font;
SDL_Color color;
bool running;
int frameCount, timerFPS, lastFrame, fps;

// Defining Ball & Paddles
SDL_Rect l_paddle, r_paddle, ball, score_board, winner_board;
std::string score;
float velX, velY;
bool turn;
int l_s, r_s;

// Serve function
void serve() {
    l_paddle.y = r_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
    if(turn) {
        ball.x = l_paddle.x + (l_paddle.w * 4);
        velX = BALL_SPEED / 2;
    } else {
        ball.x = r_paddle.x - (l_paddle.w * 4);
        velX = -BALL_SPEED / 2;
    }
    velY = 0;
    ball.y = HEIGHT / 2 - (BALL_SIZE / 2);
    turn = !turn;
}

// Write function
void write(std::string text, int x, int y) {
    SDL_Surface *surface;
    SDL_Texture *texture;
    const char* t = text.c_str();
    surface = TTF_RenderText_Solid(font, t, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    
    // Writing Scoreboard
    score_board.w = surface -> w;
    score_board.h = surface -> h;
    score_board.x = x - score_board.w;
    score_board.y = y = score_board.h;

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &score_board);
    SDL_DestroyTexture(texture);
}

// Update function
void update(Mix_Chunk* sound) {
    // Checks if hit paddle or walls and updates accordingly
    if (SDL_HasIntersection(&ball, &r_paddle))
    {
        double rel = (r_paddle.y + (r_paddle.h / 2)) - (ball.y + (BALL_SIZE / 2));
        double norm = rel / (r_paddle.h / 12);
        double bounce = norm * (5 * PI / 12);
        velX = -BALL_SPEED * cos(bounce);
        velY = BALL_SPEED * -sin(bounce);
    }

    if (SDL_HasIntersection(&ball, &l_paddle))
    {
        double rel = (l_paddle.y + (l_paddle.h / 2)) - (ball.y + (BALL_SIZE / 2));
        double norm = rel / (l_paddle.h / 12);
        double bounce = norm * (5 * PI / 12);
        velX = BALL_SPEED * cos(bounce);
        velY = BALL_SPEED * -sin(bounce);
    }

    if (ball.y > r_paddle.y + (r_paddle.h / 2)) r_paddle.y += PADDLE_SPEED;

    if (ball.y < r_paddle.y + (r_paddle.h / 2)) r_paddle.y -= PADDLE_SPEED;

    if (ball.x <= 0){
        r_s++;
        Mix_PlayChannel(-1, sound, 0);
        serve();
    }

    if (ball.x + BALL_SIZE >= WIDTH) {
        l_s++;
        Mix_PlayChannel(-1, sound, 0);
        serve();
    }

    if (r_paddle.y + r_paddle.h > HEIGHT) r_paddle.y = HEIGHT - r_paddle.h;

    ball.x += velX;
    ball.y += velY;
    score = std::to_string(l_s) + " : " + std::to_string(r_s);

    if(l_paddle.y < 0) l_paddle.y = 0;

    if (l_paddle.y + l_paddle.h > HEIGHT) l_paddle.y = HEIGHT - l_paddle.h;

    if (r_paddle.y < 0) r_paddle.y = 0;

    if (ball.y <= 0 || ball.y + BALL_SIZE >= HEIGHT) velY = -velY;
}

// Input function
void input() {
    SDL_Event e;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&e)) if(e.type == SDL_QUIT) running = false;
    if(keystates[SDL_SCANCODE_ESCAPE]) running = false;
    if (keystates[SDL_SCANCODE_UP]) l_paddle.y -= PADDLE_SPEED;
    if (keystates[SDL_SCANCODE_DOWN]) l_paddle.y += PADDLE_SPEED;
}

// Render function
void render() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
    SDL_RenderClear(renderer);

    frameCount++;
    timerFPS = SDL_GetTicks() - lastFrame;

    if(timerFPS < (1000/60)) {
        SDL_Delay((1000/60) - timerFPS);
    }

    // Writes everything to the screen
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &l_paddle);
    SDL_RenderFillRect(renderer, &r_paddle);
    SDL_RenderFillRect(renderer, &ball);

    write(score, WIDTH / 2 + FONT_SIZE, FONT_SIZE * 2);
    SDL_RenderPresent(renderer);
}

// Main function
int main(int argv, char** args) {
    // Checking to make sure everything is workiong properly
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cerr << "Failed at SDL_Init(): " << SDL_GetError() << std::endl;
    if(Mix_Init(0) < 0) std::cerr << "Failed at Mix_init()" << std::endl;
    if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cerr << "Failed at SDL_CreateWindowAndRenderer(): " << SDL_GetError() << std::endl;

    // Loading & Checking Font
    TTF_Init();
    font = TTF_OpenFont("assets/fonts/peepo.ttf", FONT_SIZE);
    if(font == NULL) std::cerr << "Failed at TTF_OpenFont()" << std::endl;

    // Loading & Checking Audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

    Mix_Chunk* sound = Mix_LoadWAV("assets/sounds/score.wav");

    if (!sound)
    {
        std::cerr << "Failed at Mix_LoadWAV(): " << Mix_GetError() << std::endl;
    }

    running = true;
    static int lastTime = 0;

    // Setting up the paddles, and the ball
    color.r = color.g = color.b = 255;
    l_s = r_s = 0;
    l_paddle.x = 32; l_paddle.h = HEIGHT / 4;
    l_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
    l_paddle.w = 16;
    r_paddle = l_paddle;
    r_paddle.x = WIDTH - r_paddle.w - 32;
    ball.w = ball.h = BALL_SIZE;

    serve();

    // Executes while program is running (after above code has run)
    while(running) {
        lastFrame = SDL_GetTicks();
        if(lastFrame >= (lastTime + 1000)) {
            lastTime = lastFrame;
            fps = frameCount;
            frameCount = 0;
        }
        update(sound);
        input();
        render();
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}
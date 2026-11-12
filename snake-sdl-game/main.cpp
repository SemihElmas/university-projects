#include <SDL.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define AREA_WIDTH 640
#define WINDOW_WIDTH 840  // Increased from 640 to 840
#define WINDOW_HEIGHT 480
#define CELL_SIZE 20
#define MAX_SNAKE_LENGTH 100
#define FONT_CHAR_SIZE 8  // Each character is 8x8 pixels
#define SPEEDUP_INTERVAL 15000  // Speed up every 15 seconds
#define SPEED_MULTIPLIER 0.8    // Speed increases by 20%
#define INITIAL_DELAY 100       // Initial delay in milliseconds

typedef struct {
    int x, y;
} Point;

// Global variables
Point snake[MAX_SNAKE_LENGTH];
Point food;
int snake_length = 5;
bool game_over = false;
bool waiting_for_restart = false;
int dx = 1, dy = 0;
Uint32 start_time;
SDL_Surface* font_surface = NULL;
SDL_Texture* font_texture = NULL;

// Function prototypes
void spawn_food();
void reset_game();
bool check_self_collision(Point head);
bool check_wall_collision(Point head);
void auto_turn();
void move_snake();
void render_food(SDL_Renderer* renderer);
void handle_input(SDL_Event* e);
void render_feature_list(SDL_Renderer* renderer);
void render_elapsed_time(SDL_Renderer* renderer);
void render_game_over(SDL_Renderer* renderer);
void render_game(SDL_Renderer* renderer);

void handle_input(SDL_Event* e) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
        case SDLK_ESCAPE:
            game_over = true;
            break;
        case SDLK_n:
            reset_game();
            break;
        case SDLK_UP:
            if (dy == 0) { dx = 0; dy = -1; }
            break;
        case SDLK_DOWN:
            if (dy == 0) { dx = 0; dy = 1; }
            break;
        case SDLK_LEFT:
            if (dx == 0) { dx = -1; dy = 0; }
            break;
        case SDLK_RIGHT:
            if (dx == 0) { dx = 1; dy = 0; }
            break;
        }
    }
}
void move_snake() {
    Point new_head = { snake[0].x + dx * CELL_SIZE, snake[0].y + dy * CELL_SIZE };

    if (check_self_collision(new_head)) {
        game_over = true;
        waiting_for_restart = true;
        return;
    }

    if (check_wall_collision(new_head)) {
        auto_turn();
        new_head.x = snake[0].x + dx * CELL_SIZE;
        new_head.y = snake[0].y + dy * CELL_SIZE;
        if (check_wall_collision(new_head)) {
            game_over = true;
            waiting_for_restart = true;
            return;
        }
    }

    // Check if the snake eats the food
    if (new_head.x == food.x && new_head.y == food.y) {
        if (snake_length < MAX_SNAKE_LENGTH) {
            snake_length++;  // Increase the snake's length
        }
        spawn_food();  // Spawn new food
    }

    // Move the snake body
    for (int i = snake_length - 1; i > 0; --i) {
        snake[i] = snake[i - 1];
    }
    snake[0] = new_head;
}

bool check_self_collision(Point head) {
    for (int i = 1; i < snake_length; ++i) {
        if (snake[i].x == head.x && snake[i].y == head.y) {
            return true;
        }
    }
    return false;
}

bool check_wall_collision(Point head) {
    return head.x < CELL_SIZE || head.x >= (640 - CELL_SIZE) ||  // Adjust width boundary
        head.y < CELL_SIZE || head.y >= (WINDOW_HEIGHT - CELL_SIZE);  // Height remains unchanged
}

void auto_turn() {
    int right_dx = dy;
    int right_dy = -dx;
    Point right_turn = { snake[0].x + right_dx * CELL_SIZE, snake[0].y + right_dy * CELL_SIZE };
    if (!check_wall_collision(right_turn)) {
        dx = right_dx;
        dy = right_dy;
        return;
    }

    int left_dx = -dy;
    int left_dy = dx;
    Point left_turn = { snake[0].x + left_dx * CELL_SIZE, snake[0].y + left_dy * CELL_SIZE };
    if (!check_wall_collision(left_turn)) {
        dx = left_dx;
        dy = left_dy;
    }
}

void spawn_food() {//makes food spawn in random location inside of the playing area
    food.x = (rand() % ((AREA_WIDTH - 2 * CELL_SIZE) / CELL_SIZE)) * CELL_SIZE + CELL_SIZE;  // Width is limited to 640 (playing area width)
    food.y = (rand() % ((WINDOW_HEIGHT - 2 * CELL_SIZE) / CELL_SIZE)) * CELL_SIZE + CELL_SIZE;

    for (int i = 0; i < snake_length; ++i) {
        if (snake[i].x == food.x && snake[i].y == food.y) {
            spawn_food();  // Retry if food spawns on the snake
            return;
        }
    }
}


void render_food(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Blue color
    SDL_Rect food_rect = { food.x, food.y, CELL_SIZE, CELL_SIZE };
    SDL_RenderFillRect(renderer, &food_rect);
}


void render_elapsed_time(SDL_Renderer* renderer) {
    Uint32 elapsed = (SDL_GetTicks() - start_time) / 1000;
    char time_text[32];
    snprintf(time_text, sizeof(time_text), "Time: %u s", elapsed);

    SDL_Rect src_rect = { 0, 0, FONT_CHAR_SIZE, FONT_CHAR_SIZE };
    SDL_Rect dest_rect = { WINDOW_WIDTH - 100, 10, FONT_CHAR_SIZE, FONT_CHAR_SIZE };

    for (int i = 0; time_text[i] != '\0'; ++i) {
        char c = time_text[i];
        src_rect.x = (c % 16) * FONT_CHAR_SIZE;
        src_rect.y = (c / 16) * FONT_CHAR_SIZE;
        dest_rect.x = WINDOW_WIDTH - 100 + i * FONT_CHAR_SIZE;
        SDL_RenderCopy(renderer, font_texture, &src_rect, &dest_rect);
    }
}

void render_feature_list(SDL_Renderer* renderer) {
    const char* features[] = {
        "1. Graphic Design",
        "2. Basic Movement",
        "3. Self Hitting",
        "4. Timer",
        "A. Lengthening snake",
        "B. Speed up"
    };

    SDL_Rect src_rect = { 0, 0, FONT_CHAR_SIZE, FONT_CHAR_SIZE };

    // Position the list in the new space to the right of the playing area
    int start_x = AREA_WIDTH + 20;  // Starts just after the right border (640px + padding)
    int start_y = CELL_SIZE + 20;  // Below the top border

    for (int i = 0; i < 6; ++i) {  // Update loop to iterate through all features
        const char* line = features[i];
        for (int j = 0; line[j] != '\0'; ++j) {
            char c = line[j];
            src_rect.x = (c % 16) * FONT_CHAR_SIZE;
            src_rect.y = (c / 16) * FONT_CHAR_SIZE;

            SDL_Rect dest_rect = {
                start_x + j * FONT_CHAR_SIZE,
                start_y + i * (FONT_CHAR_SIZE + 5),
                FONT_CHAR_SIZE,
                FONT_CHAR_SIZE
            };
            SDL_RenderCopy(renderer, font_texture, &src_rect, &dest_rect);
        }
    }
}




void render_game_over(SDL_Renderer* renderer) {
    const char* msg = "Game Over! Press 'N' to Restart or 'ESC' to Quit.";
    SDL_Rect src_rect = {0, 0, FONT_CHAR_SIZE, FONT_CHAR_SIZE};
    SDL_Rect dest_rect = {WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2, FONT_CHAR_SIZE, FONT_CHAR_SIZE};
    render_feature_list(renderer);

    for (int i = 0; msg[i] != '\0'; ++i) {
        char c = msg[i];
        src_rect.x = (c % 16) * FONT_CHAR_SIZE;
        src_rect.y = (c / 16) * FONT_CHAR_SIZE;
        dest_rect.x = (WINDOW_WIDTH / 4) + i * FONT_CHAR_SIZE;
        SDL_RenderCopy(renderer, font_texture, &src_rect, &dest_rect);
    }
}
void render_game(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    render_food(renderer);

    // Draw white borders only around the playing area (640x480)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect top_border = { 0, 0, 640, CELL_SIZE };
    SDL_Rect bottom_border = { 0, WINDOW_HEIGHT - CELL_SIZE, 640, CELL_SIZE };
    SDL_Rect left_border = { 0, 0, CELL_SIZE, WINDOW_HEIGHT };
    SDL_Rect right_border = { 640 - CELL_SIZE, 0, CELL_SIZE, WINDOW_HEIGHT };
    SDL_RenderFillRect(renderer, &top_border);
    SDL_RenderFillRect(renderer, &bottom_border);
    SDL_RenderFillRect(renderer, &left_border);
    SDL_RenderFillRect(renderer, &right_border);

    render_elapsed_time(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 0; i < snake_length; ++i) {
        SDL_Rect rect = { snake[i].x, snake[i].y, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    // Render the feature list in the expanded area
    render_feature_list(renderer);

    SDL_RenderPresent(renderer);
}

void reset_game() {
    snake_length = 5;
    for (int i = 0; i < snake_length; ++i) {
        snake[i].x = 20 - i * CELL_SIZE;
        snake[i].y = 20;
    }
    dx = 1;
    dy = 0;
    game_over = false;
    waiting_for_restart = false;
    start_time = SDL_GetTicks();
    spawn_food();  // Spawn the first piece of food
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font_surface = SDL_LoadBMP("cs8x8.bmp");
    if (!font_surface) {
    printf("Could not load cs8x8.bmp: %s\n", SDL_GetError());
    return 1;
    }
    font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
    SDL_FreeSurface(font_surface);

    reset_game();

    SDL_Event e;
    Uint32 last_speedup_time = SDL_GetTicks();
    int current_delay = INITIAL_DELAY;

    while (true) {
        if (!game_over) {
            while (SDL_PollEvent(&e) != 0) {
                handle_input(&e);
            }

            // Check for speed-up interval
            Uint32 current_time = SDL_GetTicks();
            if (current_time - last_speedup_time >= 10000) {  // Every 10 seconds
                current_delay = (int)(current_delay * SPEED_MULTIPLIER);
                if (current_delay < 10) {
                    current_delay = 10;  // Set a minimum delay to avoid freezing
                }
                last_speedup_time = current_time;
            }

            move_snake();
            render_game(renderer);
            SDL_Delay(current_delay);
        }
        else {
            render_game(renderer);
            render_game_over(renderer);
            SDL_RenderPresent(renderer);

            while (waiting_for_restart) {
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_KEYDOWN) {
                        if (e.key.keysym.sym == SDLK_n) {
                            reset_game();  // Restart the game
                            current_delay = INITIAL_DELAY;  // Reset speed
                            last_speedup_time = SDL_GetTicks();
                        }
                        else if (e.key.keysym.sym == SDLK_ESCAPE) {
                            waiting_for_restart = false;
                            game_over = true;  // Exit the game
                        }
                    }
                }
                SDL_Delay(100);
            }

            if (game_over) {
                break;  // Exit the main loop and quit the game
            }
        }
    }

    SDL_DestroyTexture(font_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
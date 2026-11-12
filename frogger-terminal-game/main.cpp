#include <curses.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define TICK_INTERVAL 100
#define NUM_OBSTACLES 5

// Structures
struct Car {
    int x;          // X position of the car on the game grid
    int y;          // Y position of the car
    int speed;      // Speed of the car
    char symbol;    // Character symbol to represent the car
    int colorPair;  // Color pair for the car
    bool isFriendly; // If true, transports the frog
};

struct Frog {
    int x;          // X position of the frog
    int y;          // Y position of the frog
    char symbol;    // Character symbol to represent the frog
    int lives;      // Number of lives left for the frog
    int colorPair;  // Color pair for the frog
    int points;     // Points for the game ranking
};

struct GameSettings {
    int playingFieldWidth; //These two lined are to create the width and height of the map
    int playingFieldHeight;
    int numCars; //number of cars
    int frogStartX; //frog positions
    int frogStartY;
    int frogLives; //howmany times frog can get hit by a car
    int carSpeedMin;
    int carSpeedMax;
    int timer; //time counter that counts backwards from given time
    int carColorPair; //colors of the characters of the game and map
    int frogColorPair;
    int borderColorPair;
    int friendlyCarColorPair;  
};

// Obstacle placements
int obstacles[NUM_OBSTACLES][2] = { {10, 5}, {30, 7}, {50, 9}, {20, 11}, {40, 13} };

// Function Declarations
void initializeCars(Car* cars, size_t numCars, const GameSettings* settings);
void moveCars(Car* cars, size_t numCars, int maxWidth, const Frog* frog, const GameSettings* settings);
void initializeFrog(Frog* frog, const GameSettings* settings);
void jumpFrog(Frog* frog, int maxWidth, int maxHeight);
void initializeScreen(const GameSettings* settings);
void drawFrog(int x, int y, char symbol, int colorPair);
void drawCars(const Car* cars, size_t numCars);
void drawObstacles();
void drawGameArea(int width, int height, int colorPair);
void drawStatus(const Frog* frog, int timer);
void handleCollision(Frog* frog, const GameSettings* settings, const Car* cars, size_t numCars);
bool checkCollision(const Frog* frog, const Car* cars, size_t numCars);
bool checkWinCondition(const Frog* frog);
void displayEndMessage(const GameSettings* settings, bool win);
void renderGame(const GameSettings* settings, const Frog* frog, const Car* cars);
void updateTimer(GameSettings* settings, clock_t* start_time);
void sleep_until(clock_t* next_tick);
void initializeGame(GameSettings* settings, Car** cars, Frog* frog);
void gameLoop(GameSettings settings, Frog* frog, Car* cars);

//Functions

//this function helps update values from the config file into the game
void mapConfigToSettings(GameSettings* settings, const char* key, int value) {
    if (strcmp(key, "playingFieldWidth") == 0) settings->playingFieldWidth = value;
    else if (strcmp(key, "playingFieldHeight") == 0) settings->playingFieldHeight = value;
    else if (strcmp(key, "numCars") == 0) settings->numCars = value;
    else if (strcmp(key, "frogStartX") == 0) settings->frogStartX = value;
    else if (strcmp(key, "frogStartY") == 0) settings->frogStartY = value;
    else if (strcmp(key, "frogLives") == 0) settings->frogLives = value;
    else if (strcmp(key, "carSpeedMin") == 0) settings->carSpeedMin = value;
    else if (strcmp(key, "carSpeedMax") == 0) settings->carSpeedMax = value;
    else if (strcmp(key, "timer") == 0) settings->timer = value;
    else if (strcmp(key, "carColorPair") == 0) settings->carColorPair = value;
    else if (strcmp(key, "frogColorPair") == 0) settings->frogColorPair = value;
    else if (strcmp(key, "borderColorPair") == 0) settings->borderColorPair = value;
    else if (strcmp(key, "friendlyCarColorPair") == 0) settings->friendlyCarColorPair = value;
}

//this function uses the mapConfigToSettings to update the game 
bool loadConfig(const char* filename, GameSettings* settings) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0 || !file) {
        fprintf(stderr, "Failed to open configuration file: %s\n", filename); //watching for any problems in loading config
        return false;
    }

    char line[256];
    char key[256];
    int value;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf_s(line, "%255[^=]=%d", key, (unsigned)sizeof(key), &value) == 2) {
            mapConfigToSettings(settings, key, value);
            printf("Loaded config: %s = %d\n", key, value); // Debug print
        }
    }

    fclose(file);
    return true;
}

//this function creates a car and gives it the values like speed symbol colorpair from the read config file
void initializeCars(Car* cars, size_t numCars, const GameSettings* settings) {
    for (size_t i = 0; i < numCars; i++) {
        cars[i].x = rand() % settings->playingFieldWidth;
        cars[i].y = (rand() % (settings->playingFieldHeight - 1)) / 2 * 2 + 1;
        cars[i].speed = ((rand() % 2) * 2 - 1) * ((rand() % (settings->carSpeedMax - settings->carSpeedMin + 1)) + settings->carSpeedMin);
        cars[i].symbol = 'C'; // Default car symbol
        cars[i].colorPair = settings->carColorPair;
        cars[i].isFriendly = (rand() % 5 == 0); // 20% chance to be friendly

        // If the car is friendly, change its color to blue
        if (cars[i].isFriendly) {
            cars[i].colorPair = settings->friendlyCarColorPair;  // Apply the friendly car color pair
        }
    }
}


// this function creates frog and gives respective values from the read config file
void initializeFrog(Frog* frog, const GameSettings* settings) {
    frog->x = settings->frogStartX;
    frog->y = settings->frogStartY;
    frog->symbol = 'F';
    frog->lives = settings->frogLives;
    frog->colorPair = settings->frogColorPair;
    frog->points = 0;
}
//this function creates the screen and creates the color pairs that will be used 
void initializeScreen(const GameSettings* settings) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color();

    // Define color pairs
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Frog
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Regular cars (Red)
    init_pair(3, COLOR_WHITE, COLOR_BLACK); // Border (White)
    init_pair(4, COLOR_BLUE, COLOR_BLACK);  // Default color for friendly cars (Blue)


}
//this function handles car movements
void moveCars(Car* cars, size_t numCars, int maxWidth, const Frog* frog, const GameSettings* settings) {
    for (size_t i = 0; i < numCars; i++) {
        if (abs(cars[i].x - frog->x) <= 2 && cars[i].y == frog->y) {
            continue; // Stop car if near frog
        }
        cars[i].x += cars[i].speed;
        if (rand() % 20 == 0) { // Change speed dynamically
            cars[i].speed = ((rand() % 2) * 2 - 1) * ((rand() % (settings->carSpeedMax - settings->carSpeedMin + 1)) + settings->carSpeedMin);
        }
        if (cars[i].x >= maxWidth || cars[i].x < 0) {
            cars[i].x = cars[i].x < 0 ? maxWidth - 1 : 0;
            cars[i].y = (rand() % (settings->playingFieldHeight - 1)) / 2 * 2 + 1;
        }
    }
}
//frog color abd synbol
void drawFrog(int x, int y, char symbol, int colorPair) {
    attron(COLOR_PAIR(colorPair));//gives color to frog
    mvaddch(y, x, symbol); //draws the frog

}

void drawCars(const Car* cars, size_t numCars) {
    for (size_t i = 0; i < numCars; i++) {
        // Check if the car is friendly
        if (cars[i].isFriendly) {
            attron(COLOR_PAIR(cars[i].colorPair)); // Apply the friendly car's color pair
        }
        else {
            attron(COLOR_PAIR(2)); // Apply regular car color (e.g., RED)
        }

        mvaddch(cars[i].y, cars[i].x, cars[i].symbol); // Draw the car
        attroff(COLOR_PAIR(cars[i].colorPair)); // Reset color pair
    }
}
void drawObstacles() {
    attron(COLOR_PAIR(3));
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        mvaddch(obstacles[i][1], obstacles[i][0], 'X');
    }

}

void drawGameArea(int width, int height, int colorPair) {
    attron(COLOR_PAIR(colorPair));
    for (int i = 0; i < width; i++) mvaddch(0, i, '='), mvaddch(height - 1, i, '-');
    for (int i = 0; i < height; i++) mvaddch(i, 0, '|'), mvaddch(i, width - 1, '|');
    mvprintw(0, width / 2 - 5, " GOAL ");
    attroff(COLOR_PAIR(colorPair));
}

void drawStatus(const Frog* frog, int timer) {
    mvprintw(0, 60, "Lives: %d  Timer: %d  Points: %d", frog->lives, timer, frog->points);
}

void handleCollision(Frog* frog, const GameSettings* settings, const Car* cars, size_t numCars) {
    if (--frog->lives > 0) {
        frog->x = settings->frogStartX;
        frog->y = settings->frogStartY;
    }
}

bool checkCollision(Frog* frog, Car* cars, size_t numCars) {
    for (size_t i = 0; i < numCars; i++) {
        if (frog->x == cars[i].x && frog->y == cars[i].y) {
            if (cars[i].isFriendly) {
                // If the car is friendly, teleport the frog to a random position
                frog->x = rand() % 80; // Random X position
                frog->y = rand() % 24; // Random Y position
                return false; // No collision (because the frog got transported)
            }
            return true; // Regular collision (frog dies)
        }
    }
    return false; // No collision
}

bool checkWinCondition(const Frog* frog) {
    return frog->y == 0;
}

void displayEndMessage(const GameSettings* settings, bool win) {
    const char* message = win ? "You Win! Press any key." :
        (settings->timer > 0 ? "Game Over! Press any key." : "Time's Up! Press any key.");
    mvprintw(settings->playingFieldHeight / 2, settings->playingFieldWidth / 2 - 10, message);
    nodelay(stdscr, FALSE);
    getch();
    endwin();
}

void renderGame(const GameSettings* settings, const Frog* frog, const Car* cars) {
    erase();
    drawGameArea(settings->playingFieldWidth, settings->playingFieldHeight, settings->borderColorPair);
    drawObstacles();
    drawFrog(frog->x, frog->y, frog->symbol, frog->colorPair);
    drawCars(cars, settings->numCars);
    drawStatus(frog, settings->timer);
    refresh();
}

void updateTimer(GameSettings* settings, clock_t* start_time) {
    if ((clock() - *start_time) / CLOCKS_PER_SEC >= 1) {
        settings->timer--;
        *start_time = clock();
    }
}

void sleep_until(clock_t* next_tick) {
    *next_tick += TICK_INTERVAL;
    clock_t now = clock();
    if (*next_tick > now) {
        napms((*next_tick - now) * 1000 / CLOCKS_PER_SEC);
    }
}

bool checkObstacleCollision(int x, int y) {
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        if (x == obstacles[i][0] && y == obstacles[i][1]) {
            return true; // Collision with obstacle
        }
    }
    return false; // No collision
}

void jumpFrog(Frog* frog, int maxWidth, int maxHeight) {
    int ch = getch(); // Non-blocking input
    int newX = frog->x;
    int newY = frog->y;

    switch (ch) {
    case KEY_UP:
        newY = frog->y - 1;
        break;
    case KEY_DOWN:
        newY = frog->y + 1;
        break;
    case KEY_LEFT:
        newX = frog->x - 1;
        break;
    case KEY_RIGHT:
        newX = frog->x + 1;
        break;
    default:
        break;
    }

    // Check for boundary conditions
    if (newX >= 0 && newX < maxWidth && newY >= 0 && newY < maxHeight) {
        // If no obstacle collision, update frog's position
        if (!checkObstacleCollision(newX, newY)) {
            frog->x = newX;
            frog->y = newY;
        }
    }
}



void initializeGame(GameSettings* settings, Car** cars, Frog* frog) {
    srand((unsigned int)time(NULL));
    *cars = (Car*)malloc(settings->numCars * sizeof(Car));
    if (!*cars) {
        fprintf(stderr, "Failed to allocate memory for cars\n");
        exit(EXIT_FAILURE);
    }
    initializeCars(*cars, settings->numCars, settings);
    initializeFrog(frog, settings);
}

void gameLoop(GameSettings settings, Frog* frog, Car* cars) {
    initializeScreen(&settings);
    clock_t next_tick = clock(), start_time = clock();

    while (settings.timer > 0 && frog->lives > 0) {
        updateTimer(&settings, &start_time);
        frog->points += 10; // Increment points for surviving
        renderGame(&settings, frog, cars);

        jumpFrog(frog, settings.playingFieldWidth, settings.playingFieldHeight);
        moveCars(cars, settings.numCars, settings.playingFieldWidth, frog, &settings);

        if (checkCollision(frog, cars, settings.numCars)) {
            handleCollision(frog, &settings, cars, settings.numCars);
            if (frog->lives == 0) break;
        }

        if (checkWinCondition(frog)) {
            frog->points += 100; // Bonus for reaching the goal
            displayEndMessage(&settings, true);
            return;
        }

        sleep_until(&next_tick);
    }

    displayEndMessage(&settings, false);
}

int main() {
    GameSettings settings;

    // Attempt to load the configuration file
    if (!loadConfig("config.txt", &settings)) {
        fprintf(stderr, "Error: Configuration file could not be loaded. Exiting.\n");
        return 1; // Exit with error code
    }

    Car* cars = nullptr;
    Frog frog;

    initializeGame(&settings, &cars, &frog);
    gameLoop(settings, &frog, cars);

    free(cars);
    endwin();
    return 0;
}
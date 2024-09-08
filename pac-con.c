#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>

#define WIDTH 40
#define HEIGHT 20
#define PACMAN '<'
#define WALL '#'
#define FOOD '.'
#define EMPTY ' '
#define DEMON 'X'
#define POWERUP 'O'
#define NUM_DEMONS 4
#define NUM_DOTS 100
#define POWERUP_DURATION 100
#define DEMON_RESPAWN_TIME 150
#define MAX_HIGH_SCORES 5
#define HIGH_SCORE_FILE "high_scores.txt"

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position pos;
    int direction;
    bool on_food;
    bool active;
    int respawn_timer;
} Demon;

typedef struct {
    char name[20];
    int score;
} HighScore;

// Global variables
int score = 0;
int food_count = 0;
bool game_over = false;
bool win = false;
Position pacman;
char board[HEIGHT][WIDTH];
Demon demons[NUM_DEMONS];
int demon_count = 0;
bool powerup_active = false;
int powerup_timer = 0;
int lives = 3;

// Function declarations
void initialize_board();
void place_walls();
void place_food_and_powerups();
void place_demons();
void initialize_game();
void draw_board();
void reset_pacman_position();
void move_pacman(int dx, int dy);
void move_demon(Demon* demon);
void respawn_demon(Demon* demon);
void update_demons();
void update_powerup();
int play_game();
void display_home_page();
int display_main_menu();
void save_high_score(int score);
void view_high_scores();

// Function implementations
//void initialize_board() {
// }

// void place_walls() {
// }

// void place_food_and_powerups() {
// }

// void place_demons() {
// }

// void initialize_game() {
// }

// void draw_board() {
// }

void reset_pacman_position() {
    board[pacman.y][pacman.x] = EMPTY;
    pacman.x = WIDTH / 2;
    pacman.y = HEIGHT / 2;
    board[pacman.y][pacman.x] = PACMAN;
}
void move_pacman(int dx, int dy) {
    int new_x = pacman.x + dx;
    int new_y = pacman.y + dy;

    if (board[new_y][new_x] != WALL) {
        if (board[new_y][new_x] == FOOD) {
            score += 10;
            food_count--;
            if (food_count == 0) {
                win = true;
                game_over = true;
                return;
            }
        } else if (board[new_y][new_x] == POWERUP) {
            powerup_active = true;
            powerup_timer = POWERUP_DURATION;
            score += 50;
        } else if (board[new_y][new_x] == DEMON) {
            if (powerup_active) {
                for (int i = 0; i < demon_count; i++) {
                    if (demons[i].pos.x == new_x && demons[i].pos.y == new_y && demons[i].active) {
                        demons[i].active = false;
                        demons[i].respawn_timer = DEMON_RESPAWN_TIME;
                        score += 200;
                        break;
                    }
                }
            } else {
                lives--;
                if (lives == 0) {
                    game_over = true;
                    return;
                }
                reset_pacman_position();
                return;
            }
        }

        board[pacman.y][pacman.x] = EMPTY;
        pacman.x = new_x;
        pacman.y = new_y;
        board[pacman.y][pacman.x] = PACMAN;
    }
}
void move_demon(Demon* demon) {
    if (!demon->active) return;

    int dx = 0, dy = 0;
    switch (demon->direction) {
        case 0: dy = -1; break;
        case 1: dx = 1; break;
        case 2: dy = 1; break;
        case 3: dx = -1; break;
    }

    int new_x = demon->pos.x + dx;
    int new_y = demon->pos.y + dy;

    if (board[new_y][new_x] != WALL && board[new_y][new_x] != DEMON) {
        if (demon->on_food) {
            board[demon->pos.y][demon->pos.x] = FOOD;
        } else {
            board[demon->pos.y][demon->pos.x] = EMPTY;
        }

        demon->pos.x = new_x;
        demon->pos.y = new_y;

        if (board[new_y][new_x] == FOOD) {
            demon->on_food = true;
        } else {
            demon->on_food = false;
        }

        if (board[new_y][new_x] == PACMAN) {
            if (!powerup_active) {
                lives--;
                if (lives == 0) {
                    game_over = true;
                } else {
                    reset_pacman_position();
                }
            }
        } else {
            board[new_y][new_x] = DEMON;
        }
    } else {
        demon->direction = rand() % 4;
    }
}

void respawn_demon(Demon* demon) {
    int x, y;
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (board[y][x] != EMPTY && board[y][x] != FOOD);

    demon->pos.x = x;
    demon->pos.y = y;
    demon->direction = rand() % 4;
    demon->on_food = (board[y][x] == FOOD);
    demon->active = true;
    demon->respawn_timer = 0;
    board[y][x] = DEMON;
}

void update_demons() {
    for (int i = 0; i < demon_count; i++) {
        if (demons[i].active) {
            move_demon(&demons[i]);
        } else {
            demons[i].respawn_timer--;
            if (demons[i].respawn_timer <= 0) {
                respawn_demon(&demons[i]);
            }
        }
    }
}

void update_powerup() {
    if (powerup_active) {
        powerup_timer--;
        if (powerup_timer == 0) {
            powerup_active = false;
        }
    }
}

// int play_game() {
// }

// void display_home_page() {
// }

int display_main_menu() {
    clear();
    printw("Main Menu\n");
    printw("1. Play the Game\n");
    printw("2. View High Scores\n");
    printw("3. Exit\n");
    printw("\nEnter your choice: ");
    refresh();
    
    int choice;
    scanf("%d", &choice);
    return choice;
}

// void save_high_score(int score) {
// }

// void view_high_scores() {
// }
// int main() {

// }

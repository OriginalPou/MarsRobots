#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum direction {NORTH,EAST,SOUTH,WEST};
typedef struct{
    int pos_x; // position x of robot
    int pos_y; // position y of robot
    enum direction direction;
    char commande[100];
    int step;
}robot;

typedef struct{
    int** map_;
    int nb_rows;
    int nb_cols;
    int nb_robots;
}mars_map;

void read_map(char *file, mars_map* mars );
void get_robots(char* file, robot* robots, int nb_robots);
enum direction find_direction(char dir);
void create_map(mars_map* mars);
void spawn_robots (mars_map* mars, robot* robots);
int get_robot_orientation (enum direction dir);
void display_map(mars_map mars);
char get_printed_character(int box);
void clearScreen();

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>

#include<stdlib.h>
#include<strings.h>
#include <unistd.h>

#define LEPORT 2020


enum direction {NORTH,EAST,SOUTH,WEST};
typedef struct{
    int pos_x; // position x of robot
    int pos_y; // position y of robot
    enum direction direction;
    char commande[100];
    int step;
    int onhold; // waiting for another robot to move
}robot;

typedef struct{
    int** map_;
    int nb_rows;
    int nb_cols;
    int nb_robots;
    int nb_robots_moving;
}mars_map;

typedef struct{
    mars_map*        mars;
    robot*           pou;
    pthread_mutex_t* dmutex;
    pthread_cond_t*  condition;
}thread_data;

typedef struct{
    struct sockaddr_in *padin; //local internet address
    struct sockaddr_in *p_exp; //pointer for sender's internet address
    int s; // socket identifier   
}serverSocket;

typedef struct{
    mars_map*        mars;
    robot*           pou;
    pthread_mutex_t* dmutex;
    pthread_cond_t*  condition;
    serverSocket*    sock;
    int              robot_nb;
}thread_server;

typedef struct{
    struct sockaddr_in  padin; //local internet address
    struct sockaddr_in *p_exp; //pointer for internet address of sender  
    int s; // socket identifier
}clientSocket;

void read_map(char *file, mars_map* mars );
void get_robots(char* file, robot* robots, int nb_robots);
enum direction find_direction(char dir);
void create_map(mars_map* mars);
void spawn_robots (mars_map* mars, robot* robots);
int get_robot_orientation (enum direction dir);
char print_direction(enum direction dir);
void display_map(mars_map mars);
char get_printed_character(int box);
void clearScreen();

int move_robot(mars_map* mars, robot* robot);
void turn_robot(mars_map* mars, robot* robot, char next_step);
void step_robot(mars_map* mars, robot* robot, char next_step);
int validate_pos(mars_map* mars,int new_x,int new_y);

void* move_robot_conc(void *arg);
void* display_map_conc(void *arg);

int menu();
void printInitial(mars_map mars, robot* robots);
void printFinal(mars_map mars, robot* robots);

void setUpClientCom(clientSocket* sock);
void setUpServer(serverSocket* sock);
void synchronize(int s,char* msg, int lg,const struct sockaddr* padin);
void waitForSync(char* message, int s, int lg, struct sockaddr * p_exp);
void getMapParams(char* message, int s, int lg, struct sockaddr * p_exp, mars_map* map);
void setMapParams(int s,char* msg, int lg,const struct sockaddr* padin, mars_map* mars);
void getRobotParams(char* message, int s, int lg, struct sockaddr * p_exp, robot* robots, int nb_robots);
void setRobotParams(int s,char* msg, int lg,const struct sockaddr* padin,robot* robots, mars_map mars);
int commandValid(char* command);
void followRobots(int s,char* msg, int lg,const struct sockaddr* padin,robot* robots, mars_map* mars);
void moveRobotServer (serverSocket* sock, robot* robots, mars_map* mars, pthread_mutex_t* dmutex, pthread_cond_t*  condition);
void *move_robot_serv(void *arg);
void *display_map_serv(void *arg);
//void moveRobotsServer (char* msg, int s, int lg, struct sockaddr * p_exp, robot* robots, mars_map* mars);

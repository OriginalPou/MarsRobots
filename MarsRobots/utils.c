#include "robot.h"

/*
//@brief: reads the file that contains details of the map
//@returns: sets the size of the map
//          and the number of the robots
*/
void read_map(char *file, mars_map* mars ) {
	FILE *f;
	char str[100];

	//open the file
	f = fopen(file,"r");
    if (f == NULL){
        printf("Can't open file %s\n",file);
        exit(-1);
    }

     //get the number of robots
    mars->nb_robots=-1;
    while (fgets(str,sizeof(str),f) != NULL){
		mars->nb_robots++;
	}

	//get number of robots still moving
    mars->nb_robots_moving=mars->nb_robots;

    //get nb of cols and rows
    f = fopen(file,"r");
    fgets(str,sizeof(str),f);
    sscanf(str, "%d %d",&mars->nb_rows,&mars->nb_cols);

    fclose(f);
}

/*
//@brief: reads the file with robot details
//@returns : array of robots initilised
*/

void get_robots(char* file, robot* robots, int nb_robots){

    FILE *f;
	char str[100];

	//open the file
	f = fopen(file,"r");
    if (f == NULL){
        printf("Can't open file %s\n",file);
        exit(-1);
    }
    //pass the first line
    fgets(str,sizeof(str),f);

    // get the robot infos
    char dir[1];
    for(int i=0;i<nb_robots; i++){
        robot pou;
        fgets(str,sizeof(str),f);
        str[strlen(str)-1] = '\0';
        sscanf(str, "%d %d %s %s",&pou.pos_x,&pou.pos_y,dir,pou.commande);
        pou.direction=find_direction(dir[0]);
        pou.step=0;// set the robot's position to 0
        pou.onhold=0; // set the onhold flag to 0
        robots[i]=pou;
    }

}

/*
//@brief: creates a 2D array of the map containig obstacles
*/

void create_map(mars_map* mars){
    const double p_obstacles = 0.1; // the map is distributed 0.6 free space and 0.4 obstacles
    int nb_obstacles = (int)mars->nb_cols*mars->nb_rows*p_obstacles;

    // creating the map
	mars->map_ = (int **)malloc(mars->nb_rows*sizeof(int *));
	for (int i=0; i< mars->nb_rows; i++){
        mars->map_[i] = (int *)malloc(mars->nb_cols*sizeof(int));
	}
    //fill the map with empty space
    for (int i=0; i<mars->nb_rows;i++){
        for(int j=0; j<mars->nb_cols;j++){
            mars->map_[i][j]=0;
        }
    }
    // add obstacles
    int obstacle_spawn;
    for (int i=0; i<nb_obstacles ;i++){
        obstacle_spawn=rand()%(mars->nb_cols*mars->nb_rows);
        mars->map_[obstacle_spawn/mars->nb_cols][obstacle_spawn%mars->nb_cols]=1;
    }

}

/*
//@brief: add the robots to the map
*/

void spawn_robots (mars_map* mars, robot* robots){
    for (int i=0; i< mars->nb_robots ; i++){
        mars->map_[robots[i].pos_y][robots[i].pos_x]= get_robot_orientation(robots[i].direction);
    }
}

/*
//@return: a specific integer that helps keep track of the orientation of the robot on the map
*/

int get_robot_orientation (enum direction dir){
    switch(dir){
        case NORTH :
            return 2;
        case EAST :
            return 3;
        case SOUTH :
            return 4;
        case WEST :
            return 5;
        default :
            return -1;
    }
}

/*
//@brief : function that displays the map, obstacles and robots
*/

void display_map(mars_map mars){
    clearScreen();
    for (int i=0; i< mars.nb_cols; i++){
        printf("=");
    }
    printf("\n");
    for (int i=0; i<mars.nb_rows; i++){
        for (int j=0; j<mars.nb_cols; j++){
            printf("%c",get_printed_character(mars.map_[i][j]));

        }
        printf("\n");

    }
    for (int i=0; i< mars.nb_cols; i++){
        printf("=");
    }
    printf("\n");
}

/*
//@brief: clears the terminal
*/

void clearScreen()
{
    int n;
    for (n = 0; n < 10; n++)
        printf( "\n\n\n\n\n\n\n\n\n\n" );
}

/*
//@brief: function that helps display the orientation of the robot
*/

char get_printed_character(int box){
    switch(box){
        case 0:
            return ' ';
        case 1:
            return '#';
        case 2:
            return 'v';// NORTH
        case 3:
            return '>';// EAST
        case 4:
            return '^';// SOUTH
        case 5:
            return '<';// WEST
        default:
            return ' ';
    }
}

/*
//@brief: function helps set the direction of the robot
*/
enum direction find_direction(char dir)
{
    switch(dir){
        case 'N':
            return NORTH;
        case 'E':
            return EAST;
        case 'S':
            return SOUTH;
        case 'W':
            return WEST;
        default :
            return -1;
    }
}

/*
//@brief: helper function for printing robot's position
*/
char print_direction(enum direction dir)
{
    switch(dir){
        case NORTH:
            return 'N';
        case EAST:
            return 'E';
        case SOUTH:
            return 'S';
        case WEST:
            return 'W';
        default :
            return -1;
    }
}

/*
* @brief: main function that moves the robot
*/

int move_robot(mars_map* mars, robot* robot)
{
    if(strlen(robot->commande)<=robot->step)
        return 0;
    char next_step=robot->commande[robot->step];
    if(next_step=='L'||next_step=='R')
        turn_robot(mars, robot, next_step);
    else
        step_robot(mars, robot, next_step);
    return 1;
}

/*
* @brief: function called by move_robot used to turn the robot
*/

void turn_robot(mars_map* mars, robot* robot, char next_step)
{
    int new_direction=robot->direction;
    if(next_step=='L'){
        new_direction++; // turn robot left
        if(new_direction>3) // this is the case when you turn left when you're headed west
            new_direction=0;
    }
    else{
        new_direction--; // turn robot right
        if(new_direction<0) // this is the case where you turn right when you're headed north
            new_direction=3;
    }
    //update direction
    robot->direction=new_direction;
    //move to the next step
    robot->step++;
    // change robot orientation on the map
    mars->map_[robot->pos_y][robot->pos_x]= get_robot_orientation(robot->direction);
}

// delta_xy helps us figure out the next move of the robot depending on its current orientation
int delta_xy[4][2] = {{0,1},{1,0},{0,-1},{-1,0}};

/*
* @brief : this function is called by move robot and is used to step the robot from one spot to another
*          the main difference in this function is that the robot waits in case the spot is held by
           another robot
*/
void step_robot(mars_map* mars, robot* robot, char next_step)
{
    int new_x=robot->pos_x, new_y=robot->pos_y;
    int* delta = delta_xy[robot->direction];
    if (next_step=='M'){
        new_x+=delta[0];
        new_y+=delta[1];
    }else{
        new_x-=delta[0];
        new_y-=delta[1];
    }
    int validation_flag = validate_pos(mars, new_x, new_y);
    //spot empty
    if(validation_flag==1){
        //empty old robot spot
        mars->map_[robot->pos_y][robot->pos_x]=0;
        //update robot position
        robot->pos_x=new_x;
        robot->pos_y=new_y;
        //update map
        mars->map_[robot->pos_y][robot->pos_x]= get_robot_orientation(robot->direction);
        //move to the next step
        robot->step++;
        //in case robot was on hold
        robot->onhold=0;

    }
    // spot on map held by another robot
    else if(validation_flag==2){
        if(robot->onhold==1){ //robot already on hold
            //move to the next step
            robot->step++;
            robot->onhold=0;
        }else
            robot->onhold=1;
    }else if(validation_flag==3){ //spot outside the map
    	turn_robot(mars, robot, 'L');
    	turn_robot(mars, robot, 'L');
    	robot->step--;
    }else
        robot->step++;
}

/*
//@brief : informs the robot if the new position is valid
//@returns : 0 if there is an obstacle
//           1 if position is free
//           2 if position is held by another robot
//	     3 if position is outside the map
*/

int validate_pos(mars_map* mars,int new_x,int new_y){
    if(new_x<0||new_x>=mars->nb_cols||new_y<0||new_y>=mars->nb_rows)
        return 3;
    int spot_on_map=mars->map_[new_y][new_x];
    if(spot_on_map==0)
        return(1);
    if(spot_on_map==1)
        return(0);
    return 2;
}

/*
* @brief: main function that moves the robot in concurrent fashion
*/

int workToDo =0; // display_map has no work to do
//mars_map* mars, robot* robot,pthread_mutex_t* dmutex, pthread_cond_t* condition
void *move_robot_conc(void *arg)
{
    printf("thread created");
    thread_data *mine= (thread_data*) arg;
    while(1){
        pthread_mutex_t* dmutex = mine->dmutex;
        pthread_cond_t * condition = mine->condition;

        pthread_mutex_lock(dmutex);
        while(workToDo==1){
            pthread_cond_wait(condition,dmutex);
        }
        mars_map* mars= mine->mars;
        robot* robot = mine->pou;
        int finish_line = move_robot(mars,robot);
        workToDo =1;
        pthread_cond_broadcast(condition);
        pthread_mutex_unlock(dmutex);
        if (finish_line==0){
            mars->nb_robots_moving--;
            pthread_exit(NULL);
        }
        sleep(1);
    }
}

/*
//@brief : function that displays the map, obstacles and robots in a concurrent fashion
*/

void *display_map_conc(void *arg){
    thread_data *mine= (thread_data*) arg;
    while(1){
        pthread_mutex_t* dmutex = mine->dmutex;
        pthread_cond_t * condition = mine->condition;
        pthread_mutex_lock(dmutex);
        while(workToDo==0){
            pthread_cond_wait(condition,dmutex);
        }
        mars_map* mars = mine->mars;
        display_map(*mars);
        workToDo=0;
        pthread_cond_broadcast(condition);
        pthread_mutex_unlock(dmutex);
        if(mars->nb_robots_moving==0){
            pthread_exit(NULL);
        }
        //sleep(1);
    }
}

/*
//@brief : function that specifies which mode we run our program on
*/
int menu(){
    printf("please be a dear and choose one of these following versions you want to run the program at\n");
    printf("1) Sequential movement of robots \n");
    printf("2) Concurrent movement of robots \n");
    printf("3) Server Client communication for robot movement control \n");
    int choice=0;
    while (choice<1 || choice >3){
        printf("enter your choice : ");
        scanf("%d", &choice);
    }
    return (choice);
}

/*
//@brief : function that prints the initial map and robot parameters
*/
void printInitial(mars_map mars, robot* robots){
    display_map(mars);
    printf("Map's height : %d Map's width : %d\n",mars.nb_rows,mars.nb_cols) ;
    for(int i=0; i<mars.nb_robots;i++){
        robot pou=robots[i];
        printf("Robot's position : %d %d %c\nCommand : %s number of steps : %ld\n", pou.pos_x, pou.pos_y, print_direction(pou.direction), pou.commande, strlen(pou.commande));
    }
    for(int32_t i=0; i<0x2FFFFFFF;i++);//delay
}

/*
//@brief : function that prints final Robot positions
*/
void printFinal(mars_map mars, robot* robots){
    printf("Final positions :\n") ;
    for(int i=0; i<mars.nb_robots;i++){
        robot pou=robots[i];
        printf("\tRobot nb %d's position : %d %d %c\n",i,pou.pos_x,pou.pos_y,print_direction(pou.direction));
    }
}



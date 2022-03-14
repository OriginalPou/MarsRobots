#include "robot.h"

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

    //get nb of cols and rows
    f = fopen(file,"r");
    fgets(str,sizeof(str),f);
    sscanf(str, "%d %d",&mars->nb_rows,&mars->nb_cols);

    fclose(f);
}

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
        pou.step=0;
        robots[i]=pou;
    }

}

void create_map(mars_map* mars){
    const double p_obstacles = 0.4; // the map is distributed 0.6 free space and 0.4 obstacles
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

void spawn_robots (mars_map* mars, robot* robots){
    for (int i=0; i< mars->nb_robots ; i++){
        mars->map_[robots[i].pos_x][robots[i].pos_y]= get_robot_orientation(robots[i].direction);
    }
}

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

void clearScreen()
{
    int n;
    for (n = 0; n < 10; n++)
        printf( "\n\n\n\n\n\n\n\n\n\n" );
}

char get_printed_character(int box){
    switch(box){
        case 0:
            return ' ';
        case 1:
            return '#';
        case 2:
            return '^';
        case 3:
            return '>';
        case 4:
            return 'v';
        case 5:
            return '<';
        default:
            return ' ';
    }
}

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

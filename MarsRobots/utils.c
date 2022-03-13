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
            break;
    }
}

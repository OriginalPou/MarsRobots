#include "robot.h"


int main()
{
    mars_map mars;
    read_map("map.txt", &mars );
    robot robots[mars.nb_robots];
    get_robots("map.txt", robots, mars.nb_robots);
    printf("%d %d\n",mars.nb_cols,mars.nb_rows) ;
    for(int i=0; i<mars.nb_robots;i++){
        robot pou=robots[i];
        printf("%d %d %s %d %d\n",pou.pos_x,pou.pos_y,pou.commande,pou.direction,strlen(pou.commande));
    }
    return 0;
}

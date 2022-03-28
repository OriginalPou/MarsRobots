#include "robot.h"


int main()
{
    srand(time(NULL));
    mars_map mars;
    read_map("map.txt", &mars );
    robot robots[mars.nb_robots];
    get_robots("map.txt", robots, mars.nb_robots);
    create_map(&mars);
    spawn_robots (&mars, robots);
    display_map(mars);
    printf("%d",mars.map_[0][5]);
    printf("%d %d\n",mars.nb_cols,mars.nb_rows) ;
    for(int i=0; i<mars.nb_robots;i++){
        robot pou=robots[i];
        printf("%d %d %s %d %d\n",pou.pos_x,pou.pos_y,pou.commande,pou.direction,strlen(pou.commande));
    }
    int flag=1;
    while(flag>=1){
        flag=0;
        for(int i=0; i<mars.nb_robots;i++){
            flag+=move_robot(&mars,&robots[i]);
        }
        display_map(mars);
        for(int32_t i=0; i<0xFFFFFFF;i++);
    }
    return 0;
}

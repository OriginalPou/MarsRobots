#include "robot.h"
pthread_mutex_t dmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition   = PTHREAD_COND_INITIALIZER;

int main()
{
    srand(time(NULL));
    mars_map mars;
    read_map("map1.txt", &mars );
    robot robots[mars.nb_robots];
    get_robots("map1.txt", robots, mars.nb_robots);
    create_map(&mars);
    spawn_robots (&mars, robots);
    display_map(mars);
    printf("%d",mars.map_[0][5]);
    printf("%d %d\n",mars.nb_cols,mars.nb_rows) ;
    for(int i=0; i<mars.nb_robots;i++){
        robot pou=robots[i];
        printf("%d %d %s %d %d\n",pou.pos_x,pou.pos_y,pou.commande,pou.direction,strlen(pou.commande));
        for(int32_t i=0; i<0xFFFFFFF;i++);// delay
    }
    //concurrent
    // create display thread

    pthread_t display;
    thread_data display_data;
    display_data.dmutex = &dmutex;
    display_data.condition = &condition;
    display_data.mars = &mars;
    pthread_create(&display,NULL,display_map_conc,(void *)&display_data);
    // create robot threads
    pthread_t robot_threads[mars.nb_robots];
    thread_data robot_data[mars.nb_robots];
    for(int i=0; i<mars.nb_robots;i++){
        robot_data[i].dmutex = &dmutex;
        robot_data[i].condition = &condition;
        robot_data[i].mars = &mars;
        robot_data[i].pou = &robots[i];
        pthread_create(&robot_threads[i],NULL,move_robot_conc,(void *)&robot_data[i]);
    }
    pthread_join(display,NULL);

    /*
    // sequential

    int flag=1;
    while(flag>=1){
        flag=0;
        for(int i=0; i<mars.nb_robots;i++){
            flag+=move_robot(&mars,&robots[i]);
        }
        display_map(mars);
        for(int32_t i=0; i<0xFFFFFFF;i++);
    }
    //display final positions needed
    */
    return 0;
}

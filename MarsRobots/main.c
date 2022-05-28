#include "robot.h"
pthread_mutex_t dmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition   = PTHREAD_COND_INITIALIZER;

int main()
{
    srand(time(NULL));
    int choice = menu();
    mars_map mars;
    read_map("map1.txt", &mars );
    robot robots[mars.nb_robots];
    get_robots("map1.txt", robots, mars.nb_robots);
    create_map(&mars);
    spawn_robots (&mars, robots);

    // print initial positions
    display_map(mars);
    printf("Map's height : %d Map's width : %d\n",mars.nb_rows,mars.nb_cols) ;
    for(int i=0; i<mars.nb_robots;i++){
        robot pou=robots[i];
        printf("Robot's position : %d %d %c\nCommand : %s number of steps : %d\n",pou.pos_x,pou.pos_y,print_direction(pou.direction),pou.commande,strlen(pou.commande));
    }
    for(int32_t i=0; i<0x2FFFFFFF;i++);//delay

    //concurrent
    if (choice ==2){
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
    }

    // sequential
    else if (choice ==1){
        int flag=1;
        int i=0;
        while(flag>=1||i<mars.nb_robots){
            flag=0;
            flag+=move_robot(&mars,&robots[i]);
            if (flag==0)
                i++;
            display_map(mars);
            for(int32_t i=0; i<0xFFFFFFF;i++);//delay
        }
    }

    //display final positions
    printf("Final positions :\n") ;
    for(int i=0; i<mars.nb_robots;i++){
        robot pou=robots[i];
        printf("\tRobot nb %d's position : %d %d %c\n",i,pou.pos_x,pou.pos_y,print_direction(pou.direction));
    }

    return 0;
}

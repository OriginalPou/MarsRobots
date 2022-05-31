#include "robot.h"

pthread_mutex_t dmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition   = PTHREAD_COND_INITIALIZER;

int main()
{
    int choice = menu();
    
    //Server Client Communication
    if (choice ==3){
    	
    	// Setting up server's side of the communication
	char msg[40];
	int bd;
	serverSocket sock;
	setUpServer(&sock);
	
	//synchronization
	printf("Waiting for synchronization\n");
	waitForSync(msg, sock.s, 40, (struct sockaddr *)sock.p_exp);
	
	//receive maps parameters
	mars_map mars;
	getMapParams(msg, sock.s, 40, (struct sockaddr *)sock.p_exp, &mars);
	
	//receive robot parameters
	robot robots[mars.nb_robots];
	getRobotParams(msg, sock.s, 40, (struct sockaddr *)sock.p_exp, robots, &mars, mars.nb_robots,1);
	create_map(&mars);
    	spawn_robots (&mars, robots);
	
	//move the robots in concurrent fashion
	while (1){
		printInitial(mars, robots);
		moveRobotServer (&sock, robots, &mars, &dmutex, &condition);
		printFinal(mars,robots);
		// receive new commands from client
		printf("Waiting for client's new commands\n");
		bd = recvfrom(sock.s,msg,40,0,(struct sockaddr *)sock.p_exp, (socklen_t*)sizeof(struct sockaddr_in));
		if (msg[0]=='N'){
			printf("No new commands, server sleeping\n");
			break;
		}
		getRobotParams(msg, sock.s, 40, (struct sockaddr *)sock.p_exp, robots, &mars, mars.nb_robots,2);
	}
    	return 1;
    }
    
    //sequential OR concurrent
    srand(time(NULL));
    mars_map mars;
    read_map("map1.txt", &mars );
    robot robots[mars.nb_robots];
    get_robots("map1.txt", robots, mars.nb_robots);
    create_map(&mars);
    spawn_robots (&mars, robots);

    // print initial positions
    printInitial(mars, robots);

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

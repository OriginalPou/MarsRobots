#include "robot.h"

void setUpServer(serverSocket* sock){
	int errno =40;
	sock->s= socket(AF_INET, SOCK_DGRAM, AF_UNSPEC);
	if(sock->s == -1)
	{
		printf("error socket creation %d\n",errno);
		exit(-1);
	}
	
	printf("socket identified as : %d \n",sock->s);

	int taille = sizeof(struct sockaddr_in);
	sock->padin  = (struct sockaddr_in *) (malloc(taille));
	bzero((char*)sock->padin,taille);
	
	sock -> padin -> sin_family = AF_INET;
	sock -> padin -> sin_port   = htons(LEPORT);

	int bd = bind(sock->s, (struct sockaddr *)sock->padin, taille);
	if(bd == -1)
	{
		printf("Attachement error : %d \n",errno);
		exit(-1);
	}

	sock->p_exp = (struct sockaddr_in *) (malloc(sizeof(struct sockaddr_in)));
	
	printf("Communication Set up from Server's side\n");
}

void setUpClientCom(clientSocket* sock){
	int s, errno;
	struct hostent *host;
	socklen_t p_lgexp = sizeof(struct sockaddr_in);
	int *p_lgexp1;
	
	char *name = "localhost";
        char *adr_serv="127.0.0.1";
	
	sock->s = socket(AF_INET, SOCK_DGRAM, AF_UNSPEC);
	if(sock->s == -1)
	{
	      printf("error socket creation %d\n",errno);
	      exit(-1);
	}
	//printf("socket identified as : %d \n",s);

	int taille = sizeof(struct sockaddr_in);
	bzero((char*)&(sock->padin),taille);

	// We get ip using gethostbyname()
	if ((host = gethostbyname(name))==NULL)
	{
		perror("Machine name");
		exit(2);
	};

	bcopy(host -> h_addr_list[0], &(sock->padin).sin_addr,host -> h_length);

	sock->padin.sin_family = AF_INET;
	sock->padin.sin_port   = htons(LEPORT);
    	
    	printf("Communication set up from client's side\n");
}

void setMapParams(int s,char* msg, int lg,const struct sockaddr* padin, mars_map* mars){
	int choice;
	printf("It's time to enter the maps' different parameters\n");
	
	//maps' height
	printf("Please enter the maps' height\n");
	do{
		printf("Enter a value from 10 to 30 :");
		scanf("%d",&choice);
	}while(!(choice>=10 & choice<=30));
	// set local maps' height
	mars->nb_rows=choice;
	printf("Sending the maps' height\n");
	sprintf(msg,"%d",choice);
	int bd = sendto(s,msg,lg,0,padin,sizeof(*padin));
	if(bd == -1)
	{
		printf("Erreur send \n");
		exit(-1);
	}
	printf("Maps' height sent\n");
	
	//maps' width
	printf("Please enter the maps'width\n");
	do{
		printf("Enter a value from 10 to 30 :");
		scanf("%d",&choice);
	}while(!(choice>=10 & choice<=30));
	// set local maps' width
	mars->nb_cols=choice;
	printf("Sending the maps' width\n");
	sprintf(msg,"%d",choice);
	bd = sendto(s,msg,lg,0,padin,sizeof(*padin));
	if(bd == -1)
	{
		printf("Erreur send \n");
		exit(-1);
	}
	printf("Maps' width sent\n");
	
	//number of robots
	printf("Please enter the number of robots you wish to manipulate\n");
	do{
		printf("Enter a value from 1 to 6 :");
		scanf("%d",&choice);
	}while(!(choice>=1 & choice<=6));
	// set local number of robots
	mars->nb_robots=choice;
	printf("Sending the number of robots\n");
	sprintf(msg,"%d",choice);
	bd = sendto(s,msg,lg,0,padin,sizeof(*padin));
	if(bd == -1)
	{
		printf("Erreur send \n");
		exit(-1);
	}
	printf("Number of robots sent\n");	
}

void getMapParams(char* message, int s, int lg, struct sockaddr * p_exp, mars_map* mars){
	socklen_t p_lgexp = sizeof(struct sockaddr_in);
	int choice;
	printf("Waiting to receive the maps' different parameters\n");
	
	//map's height
	printf("Waiting to receive map's height\n");
	int bd = recvfrom(s,message,lg,0,p_exp, &p_lgexp);
	choice = atoi(message);
	printf("map's height received equals : %d\n", choice);
	//setting local map's height
	mars->nb_rows=choice;
	
	//map's width
	printf("Waiting to receive map's width\n");
	bd = recvfrom(s,message,lg,0,p_exp, &p_lgexp);
	choice = atoi(message);
	printf("map's width received equals : %d\n", choice);
	//setting local map's width
	mars->nb_cols=choice;
	
	//number of robots
	printf("Waiting to receive the number of robots\n");
	bd = recvfrom(s,message,lg,0,p_exp, &p_lgexp);
	choice = atoi(message);
	printf("number of robots received equals : %d\n", choice);
	//setting local map's height
	mars->nb_robots=choice;
	mars->nb_robots_moving=mars->nb_robots;
}

void getRobotParams(char* message, int s, int lg, struct sockaddr * p_exp, robot* robots, int nb_robots){
	socklen_t p_lgexp = sizeof(struct sockaddr_in);
	int choice;
	printf("It's time to receive the robots' different parameters\n");
	for(int i=0; i<nb_robots;i++){
		robot pou;
		printf("Robot number %d\n",i);
		
		printf("Waiting for robot's x-axis pos\n");
		int bd = recvfrom(s,message,lg,0,p_exp, &p_lgexp);
		choice = atoi(message);
		printf("Robot's x-axis pos equals : %d\n", choice);
		//setting local robot x-axis pos
		pou.pos_x=choice;
		
		printf("Waiting for robot's y-axis pos\n");
		bd = recvfrom(s,message,lg,0,p_exp, &p_lgexp);
		choice = atoi(message);
		printf("Robot's y-axis pos equals : %d\n", choice);
		//setting local robot y-axis pos
		pou.pos_y=choice;
		
		printf("Waiting for robot's direction\n");
		bd = recvfrom(s,message,1,0,p_exp, &p_lgexp);
		printf("Robot's direction equals : %c\n", message[0]);
		//setting local robot direction
		pou.direction=find_direction(message[0]);
		
		printf("Waiting for robot's command\n");
		//setting local robot command
		bd = recvfrom(s,pou.commande,100,0,p_exp, &p_lgexp);
		printf("Robot's command equals : %s\n", pou.commande);
		
		pou.step=0;// set the robot's position to 0
        	pou.onhold=0; // set the onhold flag to 0
		robots[i]=pou;
	}
	//printf("%s",robots[0].commande);
	//int bd = sendto(s, robots[0].commande, strlen(robots[0].commande), 0, p_exp, sizeof(struct sockaddr_in));
}

void setRobotParams(int s,char* msg, int lg,const struct sockaddr* padin,robot* robots, mars_map mars){
	int choice;
	char dir;
	printf("It's time to enter the robots' parameters\n");
	for(int i=0; i<mars.nb_robots;i++){
		robot pou;
		printf("Robot number %d\n",i);
		
		//*******
		printf("Please enter robot's x-axis pos\n");
		do{
			printf("Enter a value from 0 to %d :",mars.nb_cols);
			scanf("%d",&choice);
		}while(!(choice>=0 & choice<mars.nb_cols));
		
		sprintf(msg,"%d",choice);
		int bd = sendto(s,msg,lg,0,padin,sizeof(*padin));
		if(bd == -1)
		{
			printf("Erreur send \n");
			exit(-1);
		}
		printf("robot's x-axis pos sent\n");
		//setting up local robot parmas
		pou.pos_x=choice;
		
		//*******
		printf("Please enter robot's y-axis pos\n");
		do{
			printf("Enter a value from 0 to %d :",mars.nb_rows);
			scanf("%d",&choice);
		}while(!(choice>=0 & choice<mars.nb_rows));
		
		sprintf(msg,"%d",choice);
		bd = sendto(s,msg,lg,0,padin,sizeof(*padin));
		if(bd == -1)
		{
			printf("Erreur send \n");
			exit(-1);
		}
		printf("robot's y-axis pos sent\n");
		//setting up local robot parmas
		pou.pos_y=choice;
		
		//*******
		printf("Please enter robot's direction\n");
		do{
			printf("Enter the robot's direction 'N', 'S', 'E' or 'W' ");
			scanf("%s",&dir);
		}while(!(dir=='N'||dir=='S'||dir=='E'||dir=='W'));
				
		bd = sendto(s,&dir,1,0,padin,sizeof(*padin));
		if(bd == -1)
		{
			printf("Erreur send \n");
			exit(-1);
		}
		printf("robot's direction sent\n");
		//setting up local robot parmas
		pou.direction=find_direction(dir);
		
		//*******
		printf("Please enter robot's command\n");
		do{
			printf("Enter the robot's command, a string made of 'L', 'R', 'M' or 'B' ");
			scanf("%100s",pou.commande);
		}while(!(commandValid(pou.commande)));
				
		bd = sendto(s,pou.commande,100,0,padin,sizeof(*padin));
		if(bd == -1)
		{
			printf("Erreur send \n");
			exit(-1);
		}
		printf("robot's command sent\n");
		//setting up local robot parmas
		pou.step=0;// set the robot's position to 0
        	pou.onhold=0; // set the onhold flag to 0
		robots[i]=pou;	
	}
	//int *p_lgexp1;
	//int bd= recvfrom(s, msg, lg, 0, (struct sockaddr *)padin, p_lgexp1);
	//printf("%s",msg);
}
/*
void moveRobotsServer (char* msg, int s, int lg, struct sockaddr * p_exp, robot* robots, mars_map* mars, pthread_mutex_t* dmutex, pthread_cond_t*  condition){
	int flag=1;
        int i=0;
        while(flag>=1||i<mars->nb_robots){
            flag=0;
            flag+=move_robot(mars,&robots[i]);
            sprintf(msg,"%d %d %d %c",i,robots[i].pos_x,robots[i].pos_y,print_direction(robots[i].direction));
            printf("%s\n",msg);
            int bd = sendto(s, msg, strlen(msg), 0, p_exp, sizeof(struct sockaddr_in));
            if (flag==0)
                i++;
            display_map(*mars);
            for(int32_t i=0; i<0xFFFFFFF;i++);//delay
        }
        //inform the client that that the robots' movement are fully executed
        char stop='q';
        int bd = sendto(s, &stop, 1, 0, p_exp, sizeof(struct sockaddr_in));
}*/

void moveRobotServer (serverSocket* sock, robot* robots, mars_map* mars, pthread_mutex_t* dmutex, pthread_cond_t*  condition){
	pthread_t display;
        thread_data display_data;
        display_data.dmutex = dmutex;
        display_data.condition = condition;
        display_data.mars = mars;
        pthread_create(&display,NULL,display_map_serv,(void *)&display_data);
        // create robot threads
        pthread_t robot_threads[mars->nb_robots];
        thread_server robot_data[mars->nb_robots];
        for(int i=0; i<mars->nb_robots;i++){
            robot_data[i].dmutex = dmutex;
            robot_data[i].condition = condition;
            robot_data[i].mars = mars;
            robot_data[i].pou = &robots[i];
            robot_data[i].sock = sock;
            robot_data[i].robot_nb = i;
            pthread_create(&robot_threads[i],NULL,move_robot_serv,(void *)&robot_data[i]);
        }
        pthread_join(display,NULL);
        //inform the client that that the robots' movement are fully executed
        char stop='q';
        int bd = sendto(sock->s, &stop, 1, 0, (struct sockaddr *) sock->p_exp, sizeof(struct sockaddr_in));
}

int workToDoS =0; // display_map has no work to do
void *move_robot_serv(void *arg)
{	
    	char msg[40];
    	printf("thread created");
    	thread_server *mine= (thread_server*) arg;
    	while(1){
        	pthread_mutex_t* dmutex = mine->dmutex;
        	pthread_cond_t * condition = mine->condition;

        	pthread_mutex_lock(dmutex);
        	while(workToDoS==1){
            		pthread_cond_wait(condition,dmutex);
        	}
        	mars_map* mars= mine->mars;
        	robot* robot = mine->pou;
        	serverSocket* sock = mine->sock;
        
        	int finish_line = move_robot(mars,robot);
        	sprintf(msg,"%d %d %d %c",mine->robot_nb,robot->pos_x,robot->pos_y,print_direction(robot->direction));
        	int bd = sendto(sock->s, msg, 40, 0, (struct sockaddr *) sock->p_exp, sizeof(struct sockaddr_in));
        	workToDoS =1;
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

void *display_map_serv(void *arg){
    thread_data *mine= (thread_data*) arg;
    while(1){
        pthread_mutex_t* dmutex = mine->dmutex;
        pthread_cond_t * condition = mine->condition;
        pthread_mutex_lock(dmutex);
        while(workToDoS==0){
            pthread_cond_wait(condition,dmutex);
        }
        mars_map* mars = mine->mars;
        display_map(*mars);
        workToDoS=0;
        pthread_cond_broadcast(condition);
        pthread_mutex_unlock(dmutex);
        if(mars->nb_robots_moving==0){
            pthread_exit(NULL);
        }
        //sleep(1);
    }
}

void followRobots(int s,char* msg, int lg,const struct sockaddr* padin,robot* robots, mars_map* mars){
	int *p_lgexp1;
	int robot_nb,posX,posY;
	char dir;
	while(1){
		int bd= recvfrom(s, msg, lg, 0, (struct sockaddr *)padin, p_lgexp1);
		if (msg[0]=='q')
			break;
		sscanf(msg, "%d %d %d %c",&robot_nb,&posX,&posY,&dir);
		
		//change robot's position
		robot pou;
		pou.pos_x=posX;
		pou.pos_y=posY;
		pou.direction=find_direction(dir);
		
		//empty old robot spot
        	mars->map_[robots[robot_nb].pos_y][robots[robot_nb].pos_x]=0;
        	
		robots[robot_nb]=pou;
		//change map
    		spawn_robots (mars, robots);
    		
    		//print map
    		display_map(*mars);
	}
}

int commandValid(char* command){
	for (int i=0; i<strlen(command); i++){
		if (!(command[i]=='L'||command[i]=='R'||command[i]=='B'||command[i]=='M'))
			return 0;
	}
	return 1;
}

void synchronize(int s,char* msg, int lg,const struct sockaddr* padin){
	long now;
	time(&now);
	sprintf(msg,"%ld",now);
	//printf("%s", msg);
	int bd = sendto(s,msg,lg,0,padin,sizeof(*padin));
	if(bd == -1)
	{
		printf("Erreur send \n");
		exit(-1);
	}
	printf("Server and Client synchronized\n");
}

void waitForSync(char* message, int s, int lg, struct sockaddr * p_exp){
	socklen_t p_lgexp = sizeof(struct sockaddr_in);
	int bd = recvfrom(s,message,lg,0,p_exp, &p_lgexp);
	long now;
	char *ptr;
	now = strtol(message, &ptr, 10);
	//printf("%ld\n", now);
	srand(now);
	printf("Server and Client synchronized\n");	
}
    	

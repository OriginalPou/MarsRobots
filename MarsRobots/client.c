#include "robot.h"

int main()
{
    	srand(time(NULL));
    	
    	//Set up communication
    	char msg[40];
    	clientSocket sock;
    	setUpClientCom(&sock);
    	
    	//synchronization
    	synchronize(sock.s,msg,40,(const struct sockaddr *)&(sock.padin));
    	
    	//send maps parameters
    	mars_map mars;
    	setMapParams(sock.s,msg,40,(const struct sockaddr *)&(sock.padin), &mars);
    	
    	//send robot parameters
    	robot robots[mars.nb_robots];
    	setRobotParams(sock.s,msg,40,(const struct sockaddr *)&(sock.padin),robots, mars,1);
    	
    	create_map(&mars);
    	spawn_robots (&mars, robots);
    	while (1){
    		printInitial(mars, robots);
    		followRobots(sock.s,msg,40,(const struct sockaddr *)&(sock.padin), robots, &mars);
    		printFinal(mars,robots);
    		printf("Please enter 'y' if you wish to add new commands to robots, 'n' otherwhise ");
		scanf("%1s",msg);
		msg[0]=toupper(msg[0]);
		int bd = sendto(sock.s,msg,1,0,(const struct sockaddr *)&(sock.padin),sizeof(sock.padin));
		if(bd == -1)
		{
			printf("Erreur send \n");
			exit(-1);
		}
		if (msg[0]=='N')
			break;
    	    	setRobotParams(sock.s,msg,40,(const struct sockaddr *)&(sock.padin),robots, mars,2);
    	}
    	

    return 0;
}

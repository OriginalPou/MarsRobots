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
    	setRobotParams(sock.s,msg,40,(const struct sockaddr *)&(sock.padin),robots, mars);
    	
    	create_map(&mars);
    	spawn_robots (&mars, robots);
    	
    	printInitial(mars, robots);
    	
    	followRobots(sock.s,msg,40,(const struct sockaddr *)&(sock.padin), robots, &mars);
    	
    	printFinal(mars,robots);
    	

    return 0;
}

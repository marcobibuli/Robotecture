
#define __MAIN__

#include <stdio.h>
#include <time.h>
#include "../common/network/NetworkManager.h"
#include "EuropeSim.h"

#include "../common/drivers/sim/europe/IO_europeSim.h"
#include "../common/drivers/sim/FOG_Sim.h"
#include "../common/drivers/sim/europe/GPS_AHRS_Sim.h"



bool running = true;


NetworkManager networkManager;
SimStatus* simStatus = NULL;

IO_europeSim* io_sim = NULL;
FOG_Sim* fog_sim = NULL;
GPS_AHRS_Sim* gps_ahrs_sim = NULL;



void ctrl_c_handler(int)
{
	running = false;
}


void init();
void terminate();


int main()
{
	strcpy(ROBOT_NAME, "europe");

	timespec loopSleep;
	loopSleep.tv_sec = CORE_SAMPLE_TIME_SEC;
	loopSleep.tv_nsec = CORE_SAMPLE_TIME_NSEC;
	
	simStatus = new EuropeSim("europeSimStatus");
	simStatus->create();

	if (networkManager.init() == -1)
	{
		printf("NetworkManager init error\n");
		return -1;
	}
	
	init();

	while(running)
	{
		
		
		nanosleep(&loopSleep, NULL);
	}

	terminate();

	simStatus->terminate();
	delete simStatus;
}


void init()
{
	int ret;


	io_sim = new IO_europeSim("IO_europeSim", networkManager, simStatus);
	ret = io_sim->init();
	if (ret == 0)
	{
		printf("IO_europeSim init ok\n");
		io_sim->create();
	}
	else printf("*** IO_europeSim init failed\n");


	fog_sim = new FOG_Sim("FOG_Sim", networkManager, simStatus);
	ret = fog_sim->init();
	if (ret == 0)
	{
		printf("FOG_Sim init ok\n");
		fog_sim->create();
	}
	else printf("*** FOG_Sim init failed\n");
	

	gps_ahrs_sim = new GPS_AHRS_Sim("GPS_AHRS_Sim", networkManager,simStatus);
	ret = gps_ahrs_sim->init();
	if (ret == 0)
	{
		printf("GPS_AHRS_Sim init ok\n");
		gps_ahrs_sim->create();
	}
	else printf("*** GPS_AHRS_Sim init failed\n");

}


void terminate()
{
	io_sim->terminate();
	delete io_sim;

	fog_sim->terminate();
	delete fog_sim;

	gps_ahrs_sim->terminate();
	delete gps_ahrs_sim;
}

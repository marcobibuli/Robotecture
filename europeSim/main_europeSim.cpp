
#define __MAIN__

#include <stdio.h>
#include <time.h>
#include "../common/network/NetworkManager.h"
#include "EuropeSim.h"
#include "../common/drivers/sim/GPS_AHRS_Sim.h"
#include "../common/drivers/sim/FOG_Sim.h"



bool running = true;


NetworkManager networkManager;
SimStatus* simStatus = NULL;
GPS_AHRS_Sim* gps_ahrs_sim = NULL;
FOG_Sim* fog_sim = NULL;


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

	
	gps_ahrs_sim = new GPS_AHRS_Sim("GPS_AHRS_Sim", networkManager,simStatus);
	ret = gps_ahrs_sim->init();
	if (ret == 0)
	{
		printf("GPS_AHRS_Sim init ok\n");
		gps_ahrs_sim->create();
	}
	else printf("*** GPS_AHRS_Sim init failed\n");



	fog_sim = new FOG_Sim("FOG_Sim", networkManager, simStatus);
	ret = fog_sim->init();
	if (ret == 0)
	{
		printf("FOG_Sim init ok\n");
		fog_sim->create();
	}
	else printf("*** FOG_Sim init failed\n");
	


}


void terminate()
{
	gps_ahrs_sim->terminate();
	delete gps_ahrs_sim;

	fog_sim->terminate();
	delete fog_sim;
	
}

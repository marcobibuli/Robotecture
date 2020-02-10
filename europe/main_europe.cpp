
#define __MAIN__

#include <stdio.h>
#include <string.h>

#include "Europe_status.h"
#include "Europe_telemetry.h"
#include "Europe_commands.h"

#include "../common/generic/define.h"
#include "../common/network/CommLink.h"
#include "../common/network/NetworkManager.h"

#include "../common/drivers/GPS_AHRS.h"
#include "../common/drivers/FOG.h"


bool running = true;

Europe_status *status;
Europe_telemetry* telemetry;
Europe_commands* commands;

NetworkManager networkManager;

GPS_AHRS* gps_ahrs = NULL;
FOG* fog = NULL;


void ctrl_c_handler(int)
{
	running = false;
}


void init_hardware();
void terminate_hardware();



int main()
{
	timespec loopSleep;
	loopSleep.tv_sec = CORE_SAMPLE_TIME_SEC;
	loopSleep.tv_nsec = CORE_SAMPLE_TIME_NSEC;

	strcpy(ROBOT_NAME, "europe");


	status = new Europe_status("europeStatus");
	status->create();

	
	


	if (networkManager.init() == -1)
	{
		printf("NetworkManager init error\n");
		return -1;
	}


	init_hardware();

	telemetry = new Europe_telemetry("europeTelemetry", networkManager, status);
	telemetry->create();

	commands = new Europe_commands("europeCommands");
	commands->create();


	CommLink* cl_in = new CommLink("HeartBeat_commLink_in", HARD_ACK);
	CommLink* cl_out = new CommLink("HeartBeat_commLink_out", HARD_ACK);

	

	cl_in->open(networkManager.ROBOT_IP, networkManager.HEARTHBEAT_ROBOT_PORT_IN,
		        networkManager.HMI_IP, networkManager.HEARTHBEAT_HMI_PORT_OUT);

	cl_out->open(networkManager.ROBOT_IP, networkManager.HEARTHBEAT_ROBOT_PORT_OUT,
		         networkManager.HMI_IP, networkManager.HEARTHBEAT_HMI_PORT_IN);

	cl_in->create();
	cl_out->create();
	
	
	Time_status time_status;
	int br;
	int64 msg;

	

	while (running)
	{
		
		time_status = status->time_status.get();

		
		if (cl_out->getLinkLevel() == NORMAL) cl_out->send_message((char*) & (time_status.timeStamp), sizeof(time_status.timeStamp));

		do {
			br = cl_in->recv_message((char*)&msg);
			//if (br>0) printf("%lli\n",msg);
		} while (br > 0);
		
		
		

		nanosleep(&loopSleep, NULL);
	}

	terminate_hardware();

	commands->terminate();
	telemetry->terminate();
	status->terminate();
	
}



void init_hardware()
{
	int ret;

	gps_ahrs = new GPS_AHRS("GPS_AHRS", networkManager, status->time_status, status->gps_ahrs_status);
	ret = gps_ahrs->init();
	if (ret == 0)
	{
		printf("GPS_AHRS init ok\n");
		gps_ahrs->create();
	}
	else
	{
		printf("*** GPS_AHRS init failed\n");
		exit(-1);
	}


	fog = new FOG("FOG", networkManager, status->time_status, status->fog_status);
	ret = fog->init();
	if (ret == 0)
	{
		printf("FOG init ok\n");
		fog->create();
	}
	else
	{
		printf("*** FOG init failed\n");
		exit(-1);
	}
}



void terminate_hardware()
{
	gps_ahrs->terminate();
	delete gps_ahrs;

	fog->terminate();
	delete fog;
}
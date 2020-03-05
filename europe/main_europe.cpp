
#define __MAIN__

#include <stdio.h>
#include <string.h>

#include "Europe_status.h"
#include "Europe_telemetry.h"
#include "Europe_commands.h"

#include "../common/generic/define.h"
#include "../common/network/CommLink.h"
#include "../common/network/NetworkManager.h"


#include "../common/drivers/europe/IO_europe.h"
#include "../common/drivers/FOG.h"
#include "../common/drivers/europe/GPS_AHRS.h"
#include "../common/drivers/DVL.h"
#include "../common/drivers/CTD.h"
#include "../common/drivers/PA500.h"
#include "../common/drivers/Echologger.h"
#include "../common/drivers/Pinger.h"

#include "NGC.h"
#include "Tasks.h"

#include <vector>


bool running = true;

Europe_status *status;
Europe_telemetry* telemetry;
Europe_commands* commands;

NetworkManager networkManager;

IO_europe* io = NULL;
FOG* fog = NULL;
GPS_AHRS* gps_ahrs = NULL;
DVL* dvl = NULL;
CTD* ctd = NULL;
PA500alt* pa500 = NULL;
Echologger* echologger = NULL;
Pinger* pinger = NULL;

NGC* ngc = NULL;
Tasks* tasks = NULL;



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

	commands = new Europe_commands("europeCommands", networkManager, status);
	commands->create();

	
	ngc = new NGC("NGC",networkManager, status->ngc_status, status->time_status);
	ngc->create();

	tasks = new Tasks("Tasks", networkManager, status);
	tasks->create();

	
	CommLink* cl_in = new CommLink("HeartBeat_commLink_in", HARD_ACK);
	CommLink* cl_out = new CommLink("HeartBeat_commLink_out", HARD_ACK);

	

	cl_in->open(networkManager.ROBOT_IP, networkManager.HEARTHBEAT_ROBOT_PORT_IN,
		        networkManager.HMI_IP, networkManager.HEARTHBEAT_HMI_PORT_OUT);

	cl_out->open(networkManager.ROBOT_IP, networkManager.HEARTHBEAT_ROBOT_PORT_OUT,
		         networkManager.HMI_IP, networkManager.HEARTHBEAT_HMI_PORT_IN);

	cl_in->create();
	cl_out->create();
	
	Connection_status connection_status;
	Time_status time_status;
	int br;
	int64 msg;

	Pinger_status pinger_status;

	/*
	IO_europe_status io;
	io=status->io_status.get();
	io.digitalOutput[EUROPE_DIO_EVOLOGICS_MODEM_POWER] = 1;
	status->io_status.set(io);
	*/
	
	
	

	while (running)
	{
		connection_status = status->connection_status.get();
		time_status = status->time_status.get();

		
		if (cl_out->getLinkLevel() == NORMAL)
		{
			cl_out->send_message((char*) & (time_status.timeStamp), sizeof(time_status.timeStamp));
			connection_status.ethernet_active = true;
		}
		else connection_status.ethernet_active = false;
		
		
		
		
		do {
			br = cl_in->recv_message((char*)&msg);
			//if (br>0) printf("%lli\n",msg);
		} while (br > 0);
		
		
		
		pinger_status = status->pinger_status.get();
		
		if (pinger_status.device_status==DEVICE_RUNNING) connection_status.acoustics_active = true;
		else
		{
			connection_status.acoustics_active = false;
			//strcpy(pinger_status.acoustic_tlm, "0");
			pinger_status.acoustic_tlm = "0";
			status->pinger_status.set(pinger_status);
		}
		

		
		
		status->connection_status.set(connection_status);

		

		nanosleep(&loopSleep, NULL);
	}


	terminate_hardware();



	tasks->terminate();
	delete tasks;

	ngc->terminate();
	delete ngc;

	commands->terminate();
	delete commands;

	telemetry->terminate();
	delete telemetry;

	status->terminate();
	delete status;
}



void init_hardware()
{
	int ret;
	

	io = new IO_europe("IO_europe", networkManager, status->io_status, status->dvl_status, status->pa500_status, status->echologger_status, status->pinger_status, status->time_status);
	ret = io->init();
	if (ret == 0)
	{
		printf("IO_europe init ok\n");
		io->create();
	}
	else
	{
		printf("*** IO_europe init failed\n");
		exit(-1);
	}



	fog = new FOG("FOG", networkManager, status->fog_status, status->time_status);
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



	gps_ahrs = new GPS_AHRS("GPS_AHRS", networkManager, status->gps_ahrs_status, status->time_status);
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


	
	dvl = new DVL("DVL", networkManager, status->dvl_status, status->time_status);
	ret = dvl->init();
	if (ret == 0)
	{
		printf("DVL init ok\n");
		dvl->create();
	}
	else
	{
		printf("*** DVL init failed\n");
		exit(-1);
	}



	ctd = new CTD("CTD", networkManager, status->ctd_status, status->time_status);
	ret = ctd->init();
	if (ret == 0)
	{
		printf("CTD init ok\n");
		ctd->create();
	}
	else
	{
		printf("*** CTD init failed\n");
		exit(-1);
	}



	pa500 = new PA500alt("PA500", networkManager, status->pa500_status, status->time_status);
	ret = pa500->init();
	if (ret == 0)
	{
		printf("PA500 init ok\n");
		pa500->create();
	}
	else
	{
		printf("*** PA500 init failed\n");
		exit(-1);
	}



	echologger = new Echologger("Echologger", networkManager, status->echologger_status, status->time_status);
	ret = echologger->init();
	if (ret == 0)
	{
		printf("Echologger init ok\n");
		echologger->create();
	}
	else
	{
		printf("*** Echologger init failed\n");
		exit(-1);
	}
	

	
	pinger = new Pinger("Pinger", networkManager, status->pinger_status, status->time_status);
	ret = pinger->init();
	if (ret == 0)
	{
		printf("Pinger init ok\n");
		pinger->create();
	}
	else
	{
		printf("*** Pinger init failed\n");
		exit(-1);
	}
}



void terminate_hardware()
{
	io->terminate();
	delete io;

	fog->terminate();
	delete fog;

	gps_ahrs->terminate();
	delete gps_ahrs;

	dvl->terminate();
	delete dvl;

	ctd->terminate();
	delete dvl;

	pa500->terminate();
	delete pa500;

	echologger->terminate();
	delete echologger;

	pinger->terminate();
	delete pinger;
}
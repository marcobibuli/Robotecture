/*
 * FOG_Sim.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: mx
 */


#include "FOG_Sim.h"


FOG_Sim::FOG_Sim(const char *name,NetworkManager &nm,SimStatus *s): DeviceSim(name,SCHED_FIFO,FOG_THREAD_PRIORITY,start_fogSim,nm,s)
{
	tlm = new CommLink("FOGSim_tlm", UDP_PURE);
	tlm->open(networkManager->SIM_IP, networkManager->FOG_SIM_PORT_OUT,
		      networkManager->ROBOT_IP, networkManager->FOG_ROBOT_SIM_PORT_IN);
	tlm->create();

	orientation_noise_avg = 0.0;     orientation_noise_std = 0.0;
	orientationRate_noise_avg = 0.0; orientationRate_noise_std = 0.0;

	orientation_noise = NULL;
	orientationRate_noise = NULL;

	simStatus=s;
}


FOG_Sim::~FOG_Sim()
{
	tlm->terminate();
	delete tlm;
}




int FOG_Sim::init()
{
	FILE* f;
	char name_dir_file[256];
	char value[100], value2[100];
	double val;

	sprintf(name_dir_file, "%s%s", CONFIGURATION_SIM_FILES_DIRECTORY, CONFIGURATION_FOG_FILE);

	if ((f = fopen(name_dir_file, "r")) != NULL)
	{
		while (!feof(f))
		{
			fscanf(f, "%s %s %lf\n", value, value2, &val);


			if (strcmp(value, "orientation_noise_avg") == 0) orientation_noise_avg = val * M_PI / 180.0;
			if (strcmp(value, "orientation_noise_std") == 0) orientation_noise_std = val * M_PI / 180.0;

			if (strcmp(value, "orientationRate_noise_avg") == 0) orientationRate_noise_avg = val * M_PI / 180.0;
			if (strcmp(value, "orientationRate_noise_std") == 0) orientationRate_noise_std = val * M_PI / 180.0;
		}

		fclose(f);
	
		orientation_noise = new std::normal_distribution<double>(orientation_noise_avg, orientation_noise_std);
		orientationRate_noise = new std::normal_distribution<double>(orientationRate_noise_avg, orientationRate_noise_std);
	}

	else
	{
		printf("FOG_Sim::init - File error: %s\n", name_dir_file);
		return -1;
	}

	return 0;
}

void FOG_Sim::execute()
{
	timespec tSleep;
	tSleep.tv_sec=FOG_SLEEP_SEC;
	tSleep.tv_nsec=FOG_SLEEP_NSEC;


	while(running)
	{
		send_tlm();

		nanosleep(&tSleep,NULL);
	}
}



void FOG_Sim::read_cmd()
{

}


void FOG_Sim::send_tlm()
{
	FOG_tlm_packet msg;

	msg.roll=(int64)(simStatus->phi*(180.0/M_PI)*FOG_factor);
	msg.pitch=(int64)(simStatus->theta*(180.0/M_PI)*FOG_factor);
	msg.heading=(int64)(simStatus->psi*(180.0/M_PI)*FOG_factor);
	msg.rollRate=(int64)(simStatus->p*(180.0/M_PI)*FOG_factor);
	msg.pitchRate=(int64)(simStatus->q*(180.0/M_PI)*FOG_factor);
	msg.headingRate=(int64)(simStatus->r*(180.0/M_PI)*FOG_factor);

	tlm->send_message((char*)&msg,sizeof(msg));
}




void* start_fogSim(void *arg)
{
	FOG_Sim* pThread=(FOG_Sim*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}



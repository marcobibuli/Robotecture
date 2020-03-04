/*
 * Echologger_Sim.cpp
 *
 *  Created on: Feb 05, 2019
 *      Author: mx
 */


#include "Echologger_Sim.h"


Echologger_Sim::Echologger_Sim(const char *name,NetworkManager &nm,SimStatus *s): DeviceSim(name,SCHED_FIFO,ECHOLOGGER_THREAD_PRIORITY,start_echologgerSim,nm,s)
{
	tlm=new CommLink( "Echologger_Sim_tlm" , UDP_PURE );

	tlm->open( networkManager->SIM_IP   , networkManager->ECHOLOGGER_SIM_PORT_OUT ,
			   networkManager->ROBOT_IP , networkManager->ECHOLOGGER_ROBOT_SIM_PORT_IN );

	tlm->create();

	simStatus=s;


	range_noise_avg=0.0;	range_noise_std=0.0;

	range_noise=NULL;
}


Echologger_Sim::~Echologger_Sim()
{

}


int Echologger_Sim::init()
{
	FILE *f;
	char name_dir_file[256];
	char value[100],value2[100];
	double val;

	sprintf(name_dir_file,"%s%s",CONFIGURATION_SIM_FILES_DIRECTORY,CONFIGURATION_ECHOLOGGER_FILE);

	if((f=fopen(name_dir_file,"r"))!=NULL)
	{
		while(!feof(f))
		{
			fscanf(f,"%s %s %lf\n",value,value2,&val);

			if (strcmp(value,"range_noise_avg")==0) range_noise_avg=val;
			if (strcmp(value,"range_noise_std")==0) range_noise_std=val;
		}

		fclose(f);

		range_noise=new std::normal_distribution<double>( range_noise_avg , range_noise_std );


		return 0;
	}

	else
	{
		printf("Echologger_Sim::init - File error: %s\n",name_dir_file);
		return -1;
	}
	return 0;
}

void Echologger_Sim::execute()
{
	timespec tSleep;
	tSleep.tv_sec=ECHOLOGGER_SLEEP_SEC;
	tSleep.tv_nsec=ECHOLOGGER_SLEEP_NSEC;



	while(running)
	{
		send_tlm();

		nanosleep(&tSleep,NULL);
	}
}



void Echologger_Sim::read_cmd()
{

}


void Echologger_Sim::send_tlm()
{
	Echologger_tlm_packet msg;

	double noise;
	double range_measure;

	noise=(*range_noise)(random_gen);  range_measure=simStatus->altitude + noise;

	if (simStatus->altitude < ALTITUDE_ZERO_OFFSET + 0.1) range_measure=0.0;


	msg.range=(int64)(range_measure*Echologger_factor);


	//printf("range: %lf\n",range_measure);

	if (simStatus->status_echo == DEVICE_RUNNING)
		tlm->send_message((char*)&msg,sizeof(msg));
}




void* start_echologgerSim(void *arg)
{
	Echologger_Sim* pThread=(Echologger_Sim*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}



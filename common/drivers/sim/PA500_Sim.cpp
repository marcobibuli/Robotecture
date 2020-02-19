/*
 * PA500_Sim.cpp
 *
 *  Created on: Jan 29, 2019
 *      Author: mx
 */


#include "PA500_Sim.h"


PA500_Sim::PA500_Sim(const char *name,NetworkManager &nm,SimStatus *s): DeviceSim(name,SCHED_FIFO,PA500_THREAD_PRIORITY,start_pa500Sim,nm,s)
{
	tlm=new CommLink( "PA500_Sim_tlm" , OVERRIDE );

	tlm->open( networkManager->SIM_IP , networkManager->PA500_SIM_PORT_OUT ,
			   networkManager->ROBOT_IP , networkManager->PA500_SIM_ROBOT_PORT_IN );

	tlm->create();

	simStatus=s;


	range_noise_avg=0.0;	range_noise_std=0.0;

	range_noise=NULL;
}


PA500_Sim::~PA500_Sim()
{

}


int PA500_Sim::init()
{
	FILE *f;
	char name_dir_file[256];
	char value[100],value2[100];
	double val;

	sprintf(name_dir_file,"%s%s",CONFIGURATION_SIM_FILES_DIRECTORY,CONFIGURATION_PA500_FILE);

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
		printf("PA500_Sim::init - File error: %s\n",name_dir_file);
		return -1;
	}
	return 0;
}

void PA500_Sim::execute()
{
	timespec tSleep;
	tSleep.tv_sec=PA500_SLEEP_SEC;
	tSleep.tv_nsec=PA500_SLEEP_NSEC;



	while(running)
	{
		send_tlm();

		nanosleep(&tSleep,NULL);
	}
}



void PA500_Sim::read_cmd()
{

}


void PA500_Sim::send_tlm()
{
	PA500_tlm_packet msg;

	double noise;
	double range_measure;

	noise=(*range_noise)(random_gen);  range_measure=simStatus->altitude + noise;

	if (simStatus->altitude < ALTITUDE_ZERO_OFFSET + 0.1) range_measure=0.0;


	msg.range=(int64)(range_measure*PA500_factor);


	//printf("range: %lf\n",range_measure);

	tlm->send_message((char*)&msg,sizeof(msg));
}




void* start_pa500Sim(void *arg)
{
	PA500_Sim* pThread=(PA500_Sim*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}



/*
 * CTD_Sim.cpp
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */


#include "CTD_Sim.h"


CTD_Sim::CTD_Sim(const char *name,NetworkManager &nm,SimStatus *s): DeviceSim(name,SCHED_FIFO,CTD_THREAD_PRIORITY,start_ctdSim,nm,s)
{
	tlm=new CommLink( "CTDSim_tlm" , OVERRIDE );

	tlm->open( networkManager->SIM_IP , networkManager->CTD_SIM_PORT_OUT ,
			   networkManager->ROBOT_IP , networkManager->CTD_SIM_ROBOT_PORT_IN );

	tlm->create();

	simStatus=s;


	conductivity_noise_avg=0.0;	conductivity_noise_std=0.0;
	temperature_noise_avg=0.0;	temperature_noise_std=0.0;
	depth_noise_avg=0.0;	    depth_noise_std=0.0;
	conductivity_noise=NULL;
	temperature_noise=NULL;
	depth_noise=NULL;
}


CTD_Sim::~CTD_Sim()
{

}


int CTD_Sim::init()
{
	FILE *f;
	char name_dir_file[256];
	char value[100],value2[100];
	double val;

	sprintf(name_dir_file,"%s%s",CONFIGURATION_SIM_FILES_DIRECTORY,CONFIGURATION_CTD_FILE);

	if((f=fopen(name_dir_file,"r"))!=NULL)
	{
		while(!feof(f))
		{
			fscanf(f,"%s %s %lf\n",value,value2,&val);

			if (strcmp(value,"conductivity_noise_avg")==0) conductivity_noise_avg=val;
			if (strcmp(value,"conductivity_noise_std")==0) conductivity_noise_std=val;
			if (strcmp(value,"temperature_noise_avg")==0) temperature_noise_avg=val;
			if (strcmp(value,"temperature_noise_std")==0) temperature_noise_std=val;
			if (strcmp(value,"depth_noise_avg")==0) depth_noise_avg=val;
			if (strcmp(value,"depth_noise_std")==0) depth_noise_std=val;
		}

		fclose(f);

		conductivity_noise=new std::normal_distribution<double>( conductivity_noise_avg , conductivity_noise_std );
		temperature_noise=new std::normal_distribution<double>( temperature_noise_avg , temperature_noise_std );
		depth_noise=new std::normal_distribution<double>( depth_noise_avg , depth_noise_std );


		return 0;
	}

	else
	{
		printf("CTD_Sim::init - File error: %s\n",name_dir_file);
		return -1;
	}
	return 0;
}

void CTD_Sim::execute()
{
	timespec tSleep;
	tSleep.tv_sec=CTD_SLEEP_SEC;
	tSleep.tv_nsec=CTD_SLEEP_NSEC;



	while(running)
	{
		send_tlm();

		nanosleep(&tSleep,NULL);
	}
}



void CTD_Sim::read_cmd()
{

}


void CTD_Sim::send_tlm()
{
	CTD_tlm_packet msg;

	double noise;
	double conductivity_measure,temperature_measure,depth_measure;

	noise=(*conductivity_noise)(random_gen);    conductivity_measure=1.0 + noise;
	noise=(*temperature_noise)(random_gen);     temperature_measure=24.0 + noise;
	noise=(*depth_noise)(random_gen);           depth_measure=simStatus->z + noise;


	msg.conductivity=(int64)(conductivity_measure*FOG_factor);
	msg.temperature=(int64)(temperature_measure*FOG_factor);
	msg.depth=(int64)(depth_measure*FOG_factor);


	//printf("phi: %lf   theta: %lf   psi: %lf\n",phi_measure,theta_measure,psi_measure);

	tlm->send_message((char*)&msg,sizeof(msg));
}




void* start_ctdSim(void *arg)
{
	CTD_Sim* pThread=(CTD_Sim*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}



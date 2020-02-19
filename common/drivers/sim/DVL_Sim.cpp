/*
 * DVL_Sim.cpp
 *
 *  Created on: Jan 29, 2019
 *      Author: mx
 */


#include "DVL_Sim.h"


DVL_Sim::DVL_Sim(const char *name,NetworkManager &nm, SimStatus* s): DeviceSim(name,SCHED_FIFO,DVL_THREAD_PRIORITY,start_dvlSim,nm,s)
{
	tlm=new CommLink( "DVLSim_tlm" , UDP_PURE );

	tlm->open( networkManager->SIM_IP , networkManager->DVL_SIM_PORT_OUT ,
			   networkManager->ROBOT_IP , networkManager->DVL_ROBOT_SIM_PORT_IN );

	tlm->create();

	simStatus=s;


	range_noise_avg=0.0;	range_noise_std=0.0;
	velocity_noise_avg=0.0;	velocity_noise_std=0.0;

	range_noise=NULL;
	velocity_noise=NULL;
}


DVL_Sim::~DVL_Sim()
{
	tlm->terminate();
	delete tlm;
}


int DVL_Sim::init()
{
	FILE *f;
	char name_dir_file[256];
	char value[100],value2[100];
	double val;

	sprintf(name_dir_file,"%s%s",CONFIGURATION_SIM_FILES_DIRECTORY,CONFIGURATION_DVL_FILE);

	if((f=fopen(name_dir_file,"r"))!=NULL)
	{
		while(!feof(f))
		{
			fscanf(f,"%s %s %lf\n",value,value2,&val);

			if (strcmp(value,"range_noise_avg")==0) range_noise_avg=val;
			if (strcmp(value,"range_noise_std")==0) range_noise_std=val;
			if (strcmp(value,"velocity_noise_avg")==0) velocity_noise_avg=val;
			if (strcmp(value,"velocity_noise_std")==0) velocity_noise_std=val;
		}

		fclose(f);

		range_noise=new std::normal_distribution<double>( range_noise_avg , range_noise_std );
		velocity_noise=new std::normal_distribution<double>( velocity_noise_avg , velocity_noise_std );


		return 0;
	}

	else
	{
		printf("DVL_Sim::init - File error: %s\n",name_dir_file);
		return -1;
	}
	return 0;
}

void DVL_Sim::execute()
{
	timespec tSleep;
	tSleep.tv_sec=DVL_SLEEP_SEC;
	tSleep.tv_nsec=DVL_SLEEP_NSEC;


	while(running)
	{
		send_tlm();

		nanosleep(&tSleep,NULL);
	}
}



void DVL_Sim::read_cmd()
{

}


void DVL_Sim::send_tlm()
{
	DVL_tlm_packet msg;

	double noise;
	double range1_measure,range2_measure,range3_measure,range4_measure;
	double altitude_measure,surge_measure,sway_measure,heave_measure,error_measure;
	int isValid;

	noise=(*range_noise)(random_gen);      range1_measure=simStatus->altitude/cos(RANGE_ANGLE) + noise;
	noise=(*range_noise)(random_gen);      range2_measure=simStatus->altitude/cos(RANGE_ANGLE) + noise;
	noise=(*range_noise)(random_gen);      range3_measure=simStatus->altitude/cos(RANGE_ANGLE) + noise;
	noise=(*range_noise)(random_gen);      range4_measure=simStatus->altitude/cos(RANGE_ANGLE) + noise;

	altitude_measure=(range1_measure+range2_measure+range3_measure+range4_measure)*cos(RANGE_ANGLE)/4.0;

	noise=(*velocity_noise)(random_gen);   surge_measure=simStatus->u + simStatus->u_sea_current + noise;
	noise=(*velocity_noise)(random_gen);   sway_measure=simStatus->v + simStatus->v_sea_current + noise;
	noise=(*velocity_noise)(random_gen);   heave_measure=simStatus->w + noise;
	noise=(*velocity_noise)(random_gen);   error_measure=noise;

	isValid=1;
	if (simStatus->altitude < ALTITUDE_ZERO_OFFSET + 0.1) isValid=0;


	msg.range1=(int64)(range1_measure*DVL_factor);
	msg.range2=(int64)(range2_measure*DVL_factor);
	msg.range3=(int64)(range3_measure*DVL_factor);
	msg.range4=(int64)(range4_measure*DVL_factor);
	msg.altitude=(int64)(altitude_measure*DVL_factor);
	msg.surgeVelocity=(int64)(surge_measure*DVL_factor);
	msg.swayVelocity=(int64)(sway_measure*DVL_factor);
	msg.heaveVelocity=(int64)(heave_measure*DVL_factor);
	msg.errorVelocity=(int64)(error_measure*DVL_factor);
	msg.validVelocity=(int64)isValid;

	printf("altitude: %lf   surgeVelocity: %lf   swayVelocity: %lf   valid: %d\n",altitude_measure,surge_measure,sway_measure,isValid);

	if (simStatus->status_dvl==DEVICE_RUNNING)
		tlm->send_message((char*)&msg,sizeof(msg));
}




void* start_dvlSim(void *arg)
{
	DVL_Sim* pThread=(DVL_Sim*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}



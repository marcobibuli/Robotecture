/*
 * USBLpos.cpp
 *
 *  Created on: Jun 1, 2018
 *      Author: mx
 */

#include "USBLpos.h"


USBLpos::USBLpos(const char *name,NetworkManager &nm,SimStatus *s): DeviceSim(name,SCHED_FIFO,USBL_POS_THREAD_PRIORITY,start_usblpos,nm,s)
{
	tlm=new CommLink( "USBLpos_Link" , UDP_PURE  );

	tlm->open( networkManager->SIM_IP  , networkManager->USBL_POS_SIM_PORT_OUT ,
			   networkManager->USBL_IP , networkManager->USBL_POS_PORT_IN );

	tlm->create();
}


USBLpos::~USBLpos()
{

}


int USBLpos::init()
{
	FILE *f;
	char name_dir_file[256];
	char value[100],value2[100];
	double val;

	sprintf(name_dir_file,"%s%s",CONFIGURATION_SIM_FILES_DIRECTORY,CONFIGURATION_USBL_FILE);

	if((f=fopen(name_dir_file,"r"))!=NULL)
	{
		while(!feof(f))
		{
			fscanf(f,"%s %s %lf\n",value,value2,&val);

			if (strcmp(value,"position_noise_avg")==0) position_noise_avg=val;
			if (strcmp(value,"position_noise_std")==0) position_noise_std=val;
		}

		fclose(f);

		position_noise=new std::normal_distribution<double>( position_noise_avg , position_noise_std );

	}

	else
	{
		printf("USBLpos::init - File error: %s\n",name_dir_file);
		return -1;
	}

	return 0;
}


void USBLpos::execute()
{
	timespec tSleep;
	tSleep.tv_sec=USBL_POS_SLEEP_SEC;
	tSleep.tv_nsec=USBL_POS_SLEEP_NSEC;

	USBLpos_packet pos;

	double x_measure,y_measure,z_measure,noise;

	while(running)
	{
		noise=(*position_noise)(random_gen);   x_measure=simStatus->x + noise;
		noise=(*position_noise)(random_gen);   y_measure=simStatus->y + noise;
		noise=(*position_noise)(random_gen);   z_measure=simStatus->z + noise;

		pos.x=(int64)(x_measure * USBLpos_factor);
		pos.y=(int64)(y_measure * USBLpos_factor);
		pos.z=(int64)(z_measure * USBLpos_factor);

		//printf("x: %lf   y: %lf   z: %lf\n",simStatus->x,simStatus->y,simStatus->z);

		tlm->send_message((char*)&pos,sizeof(pos));

		nanosleep(&tSleep,NULL);
	}
}



void* start_usblpos(void *arg)
{
	USBLpos* pThread=(USBLpos*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}

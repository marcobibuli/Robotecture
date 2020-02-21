/*
 * IO_europeSim.cpp
 *
 *  Created on: Dec 13, 2017
 *      Author: mx
 */


#include "IO_europeSim.h"


IO_europeSim::IO_europeSim(const char *name,NetworkManager &nm, SimStatus*s): DeviceSim(name,SCHED_FIFO,IO_THREAD_PRIORITY,start_io_europeSim,nm,s)
{
	cmd=new CommLink( "IO_europeSim_cmd" , OVERRIDE);

	cmd->open( networkManager->SIM_IP ,   networkManager->IO_SIM_PORT_IN ,
			   networkManager->ROBOT_IP , networkManager->IO_ROBOT_SIM_PORT_OUT );

	cmd->create();
	//printf("%s  %s  %d  %d\n",networkManager->SIM_IP,networkManager->ROBOT_IP, networkManager->IO_CHANNELS_SIM_PORT_IN,networkManager->IO_CHANNELS_SIM_ROBOT_PORT_OUT);



	tlm=new CommLink( "IO_europeSim_tlm" , UDP_PURE );

	tlm->open( networkManager->SIM_IP , networkManager->IO_SIM_PORT_OUT ,
			   networkManager->ROBOT_IP , networkManager->IO_ROBOT_SIM_PORT_IN );

	tlm->create();

	simStatus=s;
}


IO_europeSim::~IO_europeSim()
{

}




int IO_europeSim::init()
{
	//FILE *f;
	char name_dir_file[256];

	//sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,CONFIGURATION_IO_FILE);
	/*
	if((f=fopen(name_dir_file,"r"))!=NULL)
	{
		while(!feof(f))
		{
			fscanf(f,"%s\n",value);
		}

		fclose(f);


		return 0;
	}

	else
	{
		printf("IO_channels::init - File error: %s\n",name_dir_file);
		return -1;
	}*/

	for (int i = 0; i < EUROPE_DIO_MAX_CHANNELS; i++)
	{
		digitalInput[i] = 0;
		digitalOutput[i] = 0;
	}

	for (int i = 0; i < EUROPE_AD_MAX_CHANNELS; i++)
	{
		analogInput[i] = 0.0;
	}

	for (int i = 0; i < EUROPE_DA_MAX_CHANNELS; i++)
	{
		analogOutput[i] = 0.0;
	}

	return 0;
}

void IO_europeSim::execute()
{
	timespec tSleep;
	tSleep.tv_sec=IO_SLEEP_SEC;
	tSleep.tv_nsec=IO_SLEEP_NSEC;

	while(running)
	{
		read_cmd();

		for(int i=0;i<EUROPE_DIO_MAX_CHANNELS;i++)
			digitalInput[i]=digitalOutput[i];

		digitalInput[EUROPE_DIO_MOTOR_VFL_FAULT]=0;
		digitalInput[EUROPE_DIO_MOTOR_VFR_FAULT]=0;
		digitalInput[EUROPE_DIO_MOTOR_VRL_FAULT]=0;
		digitalInput[EUROPE_DIO_MOTOR_VRR_FAULT]=0;

		digitalInput[EUROPE_DIO_MOTOR_HFL_FAULT]=0;
		digitalInput[EUROPE_DIO_MOTOR_HFR_FAULT]=0;
		digitalInput[EUROPE_DIO_MOTOR_HRL_FAULT]=0;
		digitalInput[EUROPE_DIO_MOTOR_HRR_FAULT]=0;

		analogInput[EUROPE_AD_INPUT_VOLTAGE] = 3.5;
		analogInput[EUROPE_AD_INTERNAL_TEMPERATURE] = 2.0;

		setStatus();

		send_tlm();

		nanosleep(&tSleep,NULL);
	}
}



void IO_europeSim::read_cmd()
{
	IO_europe_sim_cmd_packet msg;
	int ret;

	do{
		ret=cmd->recv_message((char*)&msg);
		if (ret>0)
		{
			for(int i=0;i<EUROPE_DIO_MAX_CHANNELS;i++)
			{
				digitalOutput[i]=msg.digital & 1;
				msg.digital = msg.digital>>1;
			}

			for(int i=0;i<EUROPE_DA_MAX_CHANNELS;i++)
				analogOutput[i]=((double)(msg.da[i]))/IO_factor;
		}
	}while(ret>0);
}


void IO_europeSim::send_tlm()
{

	IO_europe_tlm_packet msg;

	msg.digital=0;
	for(int i=0;i<EUROPE_DIO_MAX_CHANNELS;i++)
		msg.digital=( msg.digital << 1 )+ digitalInput[EUROPE_DIO_MAX_CHANNELS-1-i]  ;
	

	for(int i=0;i<EUROPE_AD_MAX_CHANNELS;i++)
		msg.analogInput[i]=(int64)(analogInput[i]*IO_factor);

	for(int i=0;i<EUROPE_DA_MAX_CHANNELS;i++)
		msg.analogOutput[i]=(int64)(analogOutput[i]*IO_factor);


	tlm->send_message((char*)&msg,sizeof(msg));
}


void IO_europeSim::setStatus()
{
	EuropeSim* s = (EuropeSim*)simStatus;

	int i;

	i=0;
	if (digitalInput[32+i]==1) s->Svfl=MOTOR_FAULT;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==1) s->Svfl=MOTOR_ENABLED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==0) s->Svfl=MOTOR_POWERED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==0 && digitalInput[8+i]==0) s->Svfl=MOTOR_OFF;

	i=1;
	if (digitalInput[32+i]==1) s->Svfr=MOTOR_FAULT;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==1) s->Svfr=MOTOR_ENABLED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==0) s->Svfr=MOTOR_POWERED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==0 && digitalInput[8+i]==0) s->Svfr=MOTOR_OFF;

	i=2;
	if (digitalInput[32+i]==1) s->Svrl=MOTOR_FAULT;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==1) s->Svrl=MOTOR_ENABLED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==0) s->Svrl=MOTOR_POWERED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==0 && digitalInput[8+i]==0) s->Svrl=MOTOR_OFF;

	i=3;
	if (digitalInput[32+i]==1) s->Svrr=MOTOR_FAULT;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==1) s->Svrr=MOTOR_ENABLED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==0) s->Svrr=MOTOR_POWERED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==0 && digitalInput[8+i]==0) s->Svrr=MOTOR_OFF;


	i=4;
	if (digitalInput[32+i]==1) s->Shfl=MOTOR_FAULT;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==1) s->Shfl=MOTOR_ENABLED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==0) s->Shfl=MOTOR_POWERED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==0 && digitalInput[8+i]==0) s->Shfl=MOTOR_OFF;

	i=5;
	if (digitalInput[32+i]==1) s->Shfr=MOTOR_FAULT;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==1) s->Shfr=MOTOR_ENABLED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==0) s->Shfr=MOTOR_POWERED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==0 && digitalInput[8+i]==0) s->Shfr=MOTOR_OFF;

	i=6;
	if (digitalInput[32+i]==1) s->Shrl=MOTOR_FAULT;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==1) s->Shrl=MOTOR_ENABLED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==0) s->Shrl=MOTOR_POWERED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==0 && digitalInput[8+i]==0) s->Shrl=MOTOR_OFF;

	i=7;
	if (digitalInput[32+i]==1) s->Shrr=MOTOR_FAULT;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==1) s->Shrr=MOTOR_ENABLED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==1 && digitalInput[8+i]==0) s->Shrr=MOTOR_POWERED;
	else if (digitalInput[32+i]==0 && digitalInput[i]==0 && digitalInput[8+i]==0) s->Shrr=MOTOR_OFF;


	s->Vvfl=analogOutput[EUROPE_DA_MOTOR_VFL_VREF];
	s->Vvfr=analogOutput[EUROPE_DA_MOTOR_VFR_VREF];
	s->Vvrl=analogOutput[EUROPE_DA_MOTOR_VRL_VREF];
	s->Vvrr=analogOutput[EUROPE_DA_MOTOR_VRR_VREF];
	s->Vhfl=analogOutput[EUROPE_DA_MOTOR_HFL_VREF];
	s->Vhfr=analogOutput[EUROPE_DA_MOTOR_HFR_VREF];
	s->Vhrl=analogOutput[EUROPE_DA_MOTOR_HRL_VREF];
	s->Vhrr=analogOutput[EUROPE_DA_MOTOR_HRR_VREF];



	if (digitalInput[EUROPE_DIO_DVL_POWER] == 1) s->status_dvl = DEVICE_RUNNING;
	else s->status_dvl = DEVICE_OFF;

	if (digitalInput[EUROPE_DIO_EVOLOGICS_MODEM_POWER] == 1) s->status_pinger = DEVICE_RUNNING;
	else s->status_pinger = DEVICE_OFF;

	if (digitalInput[EUROPE_DIO_PA500_1_POWER] == 1) s->status_pa500 = DEVICE_RUNNING;
	else s->status_pa500 = DEVICE_OFF;

	if (digitalInput[EUROPE_DIO_PA500_2_POWER] == 1) s->status_echo = DEVICE_RUNNING;
	else s->status_echo = DEVICE_OFF;

	//printf("dvl: %d    pa500: %d\n", s->status_dvl, s->status_pa500);

	//printf("vhfl: %lf\n",simStatus->Vhfl);
}


void* start_io_europeSim(void *arg)
{
	IO_europeSim* pThread=(IO_europeSim*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}

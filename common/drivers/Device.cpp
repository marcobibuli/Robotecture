/*
 * Device.cpp
 *
 *  Created on: Apr 13, 2017
 *      Author: mx
 */

#include "Device.h"

Device::Device(const char *name,int policy,int priority,void*(*threadRoutine)(void*),NetworkManager &nm):RobotThread(name,policy,priority,threadRoutine)
{
	device_mode=DEVICE_NOT_SET;
	device_status=DEVICE_OFF;
	sem_init(&semSync,0,1);

	networkManager=&nm;

	cmd=NULL;
	tlm=NULL;

	running_sim=true;
	running_act=true;

	missed_receive_count=0;
	lastValidTimeStamp=0;
}


Device::~Device()
{
	if (cmd != NULL)
	{
		cmd->terminate();
		delete cmd;
		cmd = NULL;
	}

	if (tlm != NULL)
	{
		tlm->terminate();
		delete tlm;
		tlm = NULL;
	}

	if (cmdSim != NULL)
	{
		cmdSim->terminate();
		delete cmdSim;
		cmdSim = NULL;
	}

	if (tlmSim != NULL)
	{
		tlmSim->terminate();
		delete tlmSim;
		tlmSim = NULL;
	}
}


void Device::terminate()
{
	if (cmd!=NULL) cmd->terminate();

	if (tlm!=NULL) tlm->terminate();

	running_sim=false;
	running_act=false;
	RobotThread::terminate();
}

int Device::init()
{
	FILE *f;
	char value[100],name_dir_file[256];
	bool sim_act=false;

	sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,configFileName);

	if((f=fopen(name_dir_file,"r"))!=NULL)
	{
		while(!feof(f) && !sim_act)
		{
			fscanf(f,"%s\n",value);
			if (strcmp(value,"SIM")==0 || strcmp(value,"ACT")==0) sim_act=true;
		}

		fclose(f);

		int ret;
		if (strcmp(value,"SIM")==0)
		{
			device_mode=DEVICE_SIM;
			ret=init_sim();
		}
		else if (strcmp(value,"ACT")==0)
		{
			device_mode=DEVICE_ACT;
			ret=init_act();
		}
		return ret;
	}

	else
	{
		printf("Device::init - File error: %s\n",name_dir_file);
		return -1;
	}
}

void Device::set_mode(int mode)
{
	running_sim=false;
	running_act=false;


	FILE *f;
	char value[100],name_dir_file[256];

	sprintf(name_dir_file,"%s%s/%s",CONFIGURATION_FILES_DIRECTORY,ROBOT_NAME,configFileName);

	if((f=fopen(name_dir_file,"w"))!=NULL)
	{
		if (mode==DEVICE_SIM) strcpy(value,"SIM");
		else if (mode==DEVICE_ACT) strcpy(value,"ACT");

		fprintf(f,"%s\n",value);

		fclose(f);

		if (mode==DEVICE_SIM) init_sim();
		else if (mode==DEVICE_ACT) init_act();
	}

	else
	{
		printf("Device::init - File error: %s\n",name_dir_file);
	}
}

void Device::execute()
{
	if (device_mode==DEVICE_SIM) execute_sim();
	else if (device_mode==DEVICE_ACT) execute_act();
}



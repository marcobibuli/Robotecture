/*
 * DeviceSim.cpp
 *
 *  Created on: Dec 13, 2017
 *      Author: mx
 */

#include "DeviceSim.h"

DeviceSim::DeviceSim(const char *name,int policy,int priority,void*(*threadRoutine)(void*),NetworkManager &nm, SimStatus* s):RobotThread(name,policy,priority,threadRoutine)
{
	sem_init(&semSync,0,1);

	networkManager=&nm;

	simStatus = s;

	cmd=NULL;
	tlm=NULL;
}


DeviceSim::~DeviceSim()
{
	if (cmd!=NULL) delete cmd;
	if (tlm!=NULL) delete tlm;
}


void DeviceSim::terminate()
{
	if (cmd!=NULL) cmd->terminate();

	if (tlm!=NULL) tlm->terminate();

	RobotThread::terminate();
}






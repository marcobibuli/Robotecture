/*
 * DeviceSim.h
 *
 *  Created on: Dec 13, 2017
 *      Author: mx
 */

#ifndef DRIVERS_DEVICESIM_H_
#define DRIVERS_DEVICESIM_H_

#include <semaphore.h>
#include <random>
#include "../../generic/RobotThread.h"
#include "../../generic/define.h"
#include "../../network/NetworkManager.h"
#include "../../network/CommLink.h"
#include "SimStatus.h"


class DeviceSim:public RobotThread
{
	protected:
		char configFileName[256];
		sem_t semSync;
		NetworkManager *networkManager;

		SimStatus* simStatus;
		

		CommLink *cmd=NULL;
		CommLink *tlm=NULL;

	public:
		DeviceSim(const char *name,int policy,int priority,void*(*threadRoutine)(void*),NetworkManager &nm, SimStatus* s);
		~DeviceSim();

		virtual void terminate();

		virtual int init()=0;

		virtual void execute()=0;
};

#endif /* DRIVERS_DEVICESIM_H_ */

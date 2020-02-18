/*
 * Device.h
 *
 *  Created on: Apr 13, 2017
 *      Author: mx
 */

#ifndef DRIVERS_DEVICE_H_
#define DRIVERS_DEVICE_H_

#include <semaphore.h>
#include "../generic/RobotThread.h"
#include "../generic/define.h"
#include "../network/NetworkManager.h"
#include "../network/CommLink.h"


enum DeviceMode
{
	DEVICE_NOT_SET=0,
	DEVICE_SIM=1,
	DEVICE_ACT=2
};

enum DeviceStatus
{
	DEVICE_OFF=0,
	DEVICE_POWERED=1,
	DEVICE_INIT=2,
	DEVICE_RUNNING=3,
	DEVICE_WARNING=4,
	DEVICE_FAULT=5

};


class Device:public RobotThread
{
	protected:
		DeviceMode device_mode;
		DeviceStatus device_status;
		char configFileName[256];
		sem_t semSync;
		NetworkManager *networkManager;

		CommLink *cmd=NULL;
		CommLink *tlm=NULL;
		CommLink* cmdSim = NULL;
		CommLink* tlmSim = NULL;


		bool running_sim;
		bool running_act;

		
		int missed_receive_count;
		uint64 lastValidTimeStamp;

	public:
		Device(const char *name,int policy,int priority,void*(*threadRoutine)(void*),NetworkManager &nm);
		~Device();

		void set_mode(int mode);

		virtual void terminate();

		virtual int init();
		virtual int init_sim()=0;
		virtual int init_act()=0;

		virtual void execute();
		virtual void execute_sim()=0;
		virtual void execute_act()=0;
};

#endif /* DRIVERS_DEVICE_H_ */

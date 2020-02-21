/*
 * Pinger.h
 *
 *  Created on: May 25, 2018
 *      Author: mx
 */

#ifndef PINGER_H_
#define PINGER_H_

#include "../drivers/Device.h"
#include "../drivers/PingerLink.h"
#include "../data/Pinger_status.h"
#include "../data/Time_status.h"
#include "../data/DataAccess.h"
#include "../network/CommLink.h"


class Pinger:public Device
{
	private:
		PingerLink *pinger;
		PingerMessage *m;

		DataAccess<Time_status>* time_access = NULL;
		DataAccess<Pinger_status>* pinger_access = NULL;


	public:
		Pinger(const char *name,NetworkManager &nm, DataAccess<Pinger_status>& Pinger_access, DataAccess<Time_status>& Time_access);
		~Pinger();

		void execute_pinger();
		int init_pinger();

		virtual int init_sim();
		virtual int init_act();

		virtual void execute_sim();
		virtual void execute_act();

		void update_device_status(int r);

		void updateStatus();

		void send_telemetry();
};

void* start_pinger(void *arg);


#endif /* PINGER_H_ */

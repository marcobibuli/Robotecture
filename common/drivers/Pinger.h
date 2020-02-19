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


class Pinger:public Device
{
	private:
		PingerLink *pinger;
		PingerMessage *m;


	public:
		Pinger(const char *name,NetworkManager &nm,Status *s);
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

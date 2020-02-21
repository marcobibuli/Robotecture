/*
 * Echologger.h
 *
 *  Created on: Feb 05, 2019
 *      Author: mx
 */

#ifndef DRIVERS_ECHOLOGGER_H_
#define DRIVERS_ECHOLOGGER_H_

#include "../drivers/Device.h"
#include "../generic/custom_types.h"
#include "../generic/define.h"
#include "../generic/RobotMath.h"
#include "../network/CommLink.h"
#include "../data/Echologger_status.h"
#include "../data/Time_status.h"
#include "../data/DataAccess.h"



class Echologger:public Device
{
	private:
		double range;
		int measure_flag;

		DataAccess<Time_status>* time_access = NULL;
		DataAccess<Echologger_status>* echologger_access = NULL;

	public:
		Echologger(const char *name,NetworkManager &nm, DataAccess<Echologger_status>& Echologger_access, DataAccess<Time_status>& Time_access);
		~Echologger();

		virtual int init_sim();
		virtual int init_act();


		virtual void execute_sim();
		virtual void execute_act();

		void update_device_status(int r);

		void read_sim_tlm();

		void updateStatus();

		void send_telemetry();
};


void* start_echologger(void *arg);



#endif /* DRIVERS_ECHOLOGGER_H_ */

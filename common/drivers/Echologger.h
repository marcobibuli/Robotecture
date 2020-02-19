/*
 * Echologger.h
 *
 *  Created on: Feb 05, 2019
 *      Author: mx
 */

#ifndef DRIVERS_ECHOLOGGER_H_
#define DRIVERS_ECHOLOGGER_H_

#include "../drivers/Device.h"
#include "../common/custom_types.h"
#include "../common/RobotMath.h"
#include "../network/CommLink.h"
#include "../drivers/europe/hardwareLib/e-URoPelib.h"
#include "../data/Echologger_status.h"



class Echologger:public Device
{
	private:
		double range;
		int measure_flag;

	public:
		Echologger(const char *name,NetworkManager &nm,Status *s);
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

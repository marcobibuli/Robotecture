/*
 * PA500.h
 *
 *  Created on: Jul 24, 2018
 *      Author: mx
 */

#ifndef DRIVERS_PA500_H_
#define DRIVERS_PA500_H_

#include "../drivers/Device.h"
#include "../common/custom_types.h"
#include "../common/RobotMath.h"
#include "../network/CommLink.h"
#include "../drivers/europe/hardwareLib/e-URoPelib.h"





class PA500alt:public Device
{
	private:
		char pa500SerialDeviceName[64];

		PA500 *pa500;

		double range;

		int measure_flag;

	public:
		PA500alt(const char *name,NetworkManager &nm,Status *s);
		~PA500alt();

		virtual int init_sim();
		virtual int init_act();


		virtual void execute_sim();
		virtual void execute_act();

		void update_device_status(int r);

		void read_sim_tlm();

		void updateStatus();

		void send_telemetry();
};


void* start_pa500alt(void *arg);



#endif /* DRIVERS_PA500_H_ */

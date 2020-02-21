/*
 * PA500.h
 *
 *  Created on: Jul 24, 2018
 *      Author: mx
 */

#ifndef DRIVERS_PA500_H_
#define DRIVERS_PA500_H_

#include "../drivers/Device.h"
#include "../generic/custom_types.h"
#include "../generic/define.h"
#include "../generic/RobotMath.h"
#include "../network/CommLink.h"
#include "../data/PA500_status.h"
#include "../data/Time_status.h"
#include "../data/DataAccess.h"
#include "../drivers/europe/hardwareLib/e-URoPelib.h"




class PA500alt:public Device
{
	private:
		char pa500SerialDeviceName[64];

		PA500 *pa500;

		double range;

		int measure_flag;

		DataAccess<Time_status>* time_access = NULL;
		DataAccess<PA500_status>* pa500_access = NULL;

	public:
		PA500alt(const char *name,NetworkManager &nm, DataAccess<PA500_status>& PA500_access, DataAccess<Time_status>& Time_access);
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

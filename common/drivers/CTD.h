/*
 * CTD.h
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */

#ifndef DRIVERS_CTD_H_
#define DRIVERS_CTD_H_


#include "../drivers/Device.h"
#include "../generic/custom_types.h"
#include "../generic/RobotMath.h"
#include "../network/CommLink.h"
#include "../drivers/europe/hardwareLib/e-URoPelib.h"
#include "../data/CTD_status.h"
#include "../data/Time_status.h"
#include "../data/DataAccess.h"




class CTD:public Device
{
	private:
		Ctd304Plus ctd304Plus;
		char ctd304PlusSerialDeviceName[64];
		double conductivity,temperature,depth;

		DataAccess<Time_status>* time_access = NULL;
		DataAccess<CTD_status>* ctd_access = NULL;

	public:
		CTD(const char *name,NetworkManager &nm, DataAccess<CTD_status>& CTD_access, DataAccess<Time_status>& Time_access);
		~CTD();

		virtual int init_sim();
		virtual int init_act();

		virtual void execute_sim();
		virtual void execute_act();

		void update_device_status(int r);

		void read_sim_tlm();

		void updateStatus();

		void send_telemetry();
};


void* start_ctd(void *arg);


#endif /* DRIVERS_CTD_H_ */

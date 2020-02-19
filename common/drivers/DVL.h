/*
 * DVL.h
 *
 *  Created on: Jul 24, 2018
 *      Author: mx
 */

#ifndef DRIVERS_DVL_H_
#define DRIVERS_DVL_H_



#include "Device.h"
#include "sio.h"
#include "../generic/custom_types.h"
#include "../generic/define.h"
#include "../generic/RobotMath.h"
#include "../network/CommLink.h"
#include "../data/DVL_status.h"
#include "../data/IO_europe_status.h"
#include "../data/Time_status.h"
#include "../data/DataAccess.h"

#define RANGE_ANGLE (M_PI/6.0)
#define MAX_PACKET_SIZE (324)


const int MAXIMUM_RECEIVED = 1000;





class DVL:public Device
{
	private:
		int packetLength;
		double range1,range2,range3,range4,altitude;
		double surgeVelocity,swayVelocity,heaveVelocity,errorVelocity;
		int validVelocity;

		Sio *m_pSio;
		char dvlexplorerSerialDeviceName[64];

		uint16 surge_int,sway_int,heave_int;

		double su,sv;

		int measure_flag;

		DataAccess<Time_status>* time_access = NULL;
		DataAccess<DVL_status>* dvl_access = NULL;
		DataAccess<IO_europe_status>* io_access = NULL;

		float bytesToLong(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3)
		{
			unsigned char b[4]={b3, b2, b1, b0 };
			long *d = (long *)b;
			return *d;
		}

	public:
		DVL(const char *name,NetworkManager &nm, DataAccess<DVL_status>& DVL_access, DataAccess<IO_europe_status>& IO_access, DataAccess<Time_status>& Time_access);
		~DVL();

		virtual int init_sim();
		virtual int init_act();

		virtual void execute_sim();
		virtual void execute_act();

		void update_device_status(int r);

		void read_sim_tlm();

		void updateStatus();

		void send_telemetry();

		void convertData(unsigned char *packet);
		int receive(unsigned char *recMessage);
		int initExplorerDVL();
		uint16 computeChecksum(unsigned char *dvl_packet, int pckSize, unsigned char &b1, unsigned char &b2);
};


void* start_dvl(void *arg);


#endif /* DRIVERS_DVL_H_ */

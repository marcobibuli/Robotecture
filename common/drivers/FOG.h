/*
 * FOG.h
 *
 *  Created on: Jun 6, 2018
 *      Author: mx
 */

#ifndef DRIVERS_FOG_H_
#define DRIVERS_FOG_H_


#include "Device.h"
#include "../generic/custom_types.h"
#include "../generic/RobotMath.h"
#include "../network/CommLink.h"
#include "../data/FOG_status.h"
#include "../data/Time_status.h"
#include "../data/DataAccess.h"
#include "Int24.h"


#define FOG_QUADRANS_PORT_NAV 8113

# pragma pack (push, 1)
typedef struct {
	char synch;
	uint32_t secs;
	uint16_t usecs;
	int32_t latitude;
	int32_t longitude;
	int32_t altitude;
	int16_t heave;
	int16_t Nvel; // NED velocities (North-East-Down)
	int16_t Evel; // NED velocities (North-East-Down)
	int16_t Dvel; // NED velocities (North-East-Down)
	unsigned char roll[3]; //integer 24 bit
	unsigned char pitch[3]; //integer 24 bit
	unsigned char heading[3]; //integer 24 bit
	unsigned char headingRate[3]; //integer 24 bit
	unsigned char rollRate[3]; //integer 24 bit
	unsigned char pitchRate[3]; //integer 24 bit
	uint32_t status; //integer 32 bit
	uint16_t LatStdDev;
	uint16_t LongStdDev;
	uint16_t NorthStdDev;
	uint16_t EastStdDev;
	uint16_t DownStdDev;
	uint16_t RollStdDev;
	uint16_t PitchStdDev;
	uint16_t HeadingStdDev;
	int16_t CRC; //to check packet validity
}LongBinNavHRDataStruct;
# pragma pack (pop)


typedef struct
{
	double altitude;
	double heave;
    double Nvel;        // NED velocities (North-East-Down)
    double Evel;        // NED velocities (North-East-Down)
    double Dvel;        // NED velocities (North-East-Down)
    double roll;        //degrees
    double pitch;       //degrees
    double heading;     //degrees
	double headingRate; //degrees/sec
    double rollRate;    //degrees/sec
    double pitchRate;   //degrees/sec
    bool   isValid;     //valid CRC;
} Robot_LongBinNavHR;





class FOG:public Device
{
	private:
		CommLink *mpADatagram;

		int packetLength;

		LongBinNavHRDataStruct *NavData;
		Robot_LongBinNavHR *RobotNavData;

		double AngleResolution;
		double timeUsecResolution;
		double GPSResolution;

		DataAccess<Time_status>* time_access = NULL;
		DataAccess<FOG_status>* fog_access = NULL;


	public:
		FOG(const char *name,NetworkManager &nm, DataAccess<Time_status>& Time_access, DataAccess<FOG_status>& Fog_access);
		~FOG();

		virtual int init_sim();
		virtual int init_act();

		virtual void execute_sim();
		virtual void execute_act();

		int receive(char *recMessage);
		int check_crc(unsigned char *message);
		unsigned short blkcrc(unsigned char* bufptr, unsigned len);
		void convertData();

		void updateStatus();
		void update_device_status(int r);

		void send_telemetry();
		void read_sim_tlm();
};


void* start_fog(void *arg);


#endif /* DRIVERS_FOG_H_ */

/*
 * GPS_AHRS_status.h
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */

#ifndef FOG_STATUS_H_
#define FOG_STATUS_H_

#include <math.h>
#include "../generic/custom_types.h"
#include "../generic/Variable.h"


struct FOG_tlm_packet
{
	int64 roll;
	int64 pitch;
	int64 heading;
	int64 rollRate;
	int64 pitchRate;
	int64 headingRate;
	int64 device_status;
};

class FOG_status
{
	private:

		void copy_data(const FOG_status &d)
		{
			roll=d.roll;      pitch=d.pitch;    heading=d.heading;
			rollRate =d.rollRate;    pitchRate =d.pitchRate;    headingRate =d.headingRate;
			
			device_status =d.device_status;
			timeStamp=d.timeStamp;

			compose_tlm_packet(tlm_packet);
		}


	public:

		Variable roll,pitch,heading;
		Variable rollRate, pitchRate, headingRate;
		
		int64 device_status;
		
		int64 timeStamp;

		FOG_tlm_packet tlm_packet;

		FOG_status()
		{
			roll.zero();      pitch.zero();      heading.zero();
			rollRate.zero();     pitchRate.zero();      headingRate.zero();	
			device_status =0;
			timeStamp=0;


			tlm_packet.roll=0;      tlm_packet.pitch=0;      tlm_packet.heading=0;
			tlm_packet.rollRate =0;     tlm_packet.pitchRate =0;      tlm_packet.headingRate =0;
			tlm_packet.device_status =0;
		}


		FOG_status(FOG_status&d)
		{
			copy_data(d);
		}


		~FOG_status(){}


		FOG_status& operator=(const FOG_status&d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(FOG_tlm_packet&tp)
		{
			tp.roll=(int64)(roll.value*(180.0/M_PI)*FOG_factor);
			tp.pitch=(int64)(pitch.value*(180.0/M_PI)* FOG_factor);
			tp.heading=(int64)(heading.value*(180.0/M_PI)* FOG_factor);
			tp.rollRate=(int64)(rollRate.value*(180.0/M_PI)* FOG_factor);
			tp.pitchRate=(int64)(pitchRate.value*(180.0/M_PI)* FOG_factor);
			tp.headingRate=(int64)(headingRate.value*(180.0/M_PI)* FOG_factor);
			
			tp.device_status =(int64)(device_status);
		}


		void extract_tlm_packet(FOG_tlm_packet tp)
		{
			roll.value = (((double)(tp.roll)) / FOG_factor) * M_PI / 180.0;
			pitch.value = (((double)(tp.pitch)) / FOG_factor) * M_PI / 180.0;
			heading.value = (((double)(tp.heading)) / FOG_factor) * M_PI / 180.0;

			rollRate.value = (((double)(tp.rollRate)) / FOG_factor) * M_PI / 180.0;
			pitchRate.value = (((double)(tp.pitchRate)) / FOG_factor) * M_PI / 180.0;
			headingRate.value = (((double)(tp.headingRate)) / FOG_factor) * M_PI / 180.0;

			device_status = tp.device_status;
		}
};


#endif /* FOG_STATUS_H_ */

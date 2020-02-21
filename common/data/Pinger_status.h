/*
 * Pinger_status.h
 *
 *  Created on: Jan 31, 2019
 *      Author: mx
 */

#ifndef PINGER_STATUS_H_
#define PINGER_STATUS_H_

#include "../generic/custom_types.h"
#include "../generic/Variable.h"

# pragma pack (push, 1)
struct Pinger_tlm_packet
{
	int64 x;
	int64 y;
	int64 z;
	int64 device_status;
};
# pragma pack (pop)


# pragma pack (push, 1)
struct USBLpos_packet
{
	int64 x;
	int64 y;
	int64 z;
};
# pragma pack (pop)

class Pinger_status
{
	private:

		void copy_data(const Pinger_status&d)
		{
			x=d.x;
			y=d.y;
			z=d.z;

			device_status=d.device_status;
			timeStamp=d.timeStamp;

			powered = d.powered;

			compose_tlm_packet(tlm_packet);
		}


	public:

		Variable x,y,z;

		int device_status;
		int64 timeStamp;

		int powered;

		Pinger_tlm_packet tlm_packet;

		Pinger_status()
		{
			x.zero();
			y.zero();
			z.zero();

			device_status=0;
			timeStamp=0;

			powered = 0;

			tlm_packet.x=0; tlm_packet.y=0; tlm_packet.z=0;
			tlm_packet.device_status=0;
		}


		Pinger_status(Pinger_status&d)
		{
			copy_data(d);
		}


		~Pinger_status(){}


		Pinger_status& operator=(const Pinger_status&d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(Pinger_tlm_packet &tp)
		{
			tp.x=(int64)(x.value*CTD_factor);
			tp.y=(int64)(y.value*CTD_factor);
			tp.z=(int64)(z.value*CTD_factor);
			tp.device_status=(int64)device_status;
		}
};


#endif /* PINGER_STATUS_H_ */

/*
 * CTD_status.h
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */

#ifndef CTD_STATUS_H_
#define CTD_STATUS_H_

#include "../generic/custom_types.h"
#include "../generic/Variable.h"

# pragma pack (push, 1)
struct CTD_tlm_packet
{
	int64 conductivity;
	int64 temperature;
	int64 depth;
	int64 device_status;
};
# pragma pack (pop)

class CTD_status
{
	private:

		void copy_data(const CTD_status &d)
		{
			conductivity=d.conductivity;
			temperature=d.temperature;
			depth=d.depth;

			device_status=d.device_status;
			timeStamp=d.timeStamp;

			compose_tlm_packet(tlm_packet);
		}


	public:

		Variable conductivity,temperature,depth;

		int device_status;
		int64 timeStamp;

		CTD_tlm_packet tlm_packet;

		CTD_status()
		{
			conductivity.zero();
			temperature.zero();
			depth.zero();

			device_status=0;
			timeStamp=0;

			tlm_packet.conductivity=0; tlm_packet.temperature=0; tlm_packet.depth=0;
			tlm_packet.device_status=0;
		}


		CTD_status(CTD_status &d)
		{
			copy_data(d);
		}


		~CTD_status(){}


		CTD_status& operator=(const CTD_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(CTD_tlm_packet &tp)
		{
			tp.conductivity=(int64)(conductivity.value*CTD_factor);
			tp.temperature=(int64)(temperature.value*CTD_factor);
			tp.depth=(int64)(depth.value*CTD_factor);
			tp.device_status=(int64)device_status;
		}
};


#endif /* CTD_STATUS_H_ */

/*
 * PA500_status.h
 *
 *  Created on: Jan 29, 2019
 *      Author: mx
 */

#ifndef PA500_STATUS_H_
#define PA500_STATUS_H_

#include "../generic/custom_types.h"
#include "../generic/Variable.h"

# pragma pack (push, 1)
struct PA500_tlm_packet
{
	int64 range;
	int64 device_status;
};
# pragma pack (pop)

class PA500_status
{
	private:

		void copy_data(const PA500_status &d)
		{
			range=d.range;

			device_status=d.device_status;
			timeStamp=d.timeStamp;

			powered = d.powered;

			compose_tlm_packet(tlm_packet);
		}


	public:

		Variable range;

		int device_status;
		int64 timeStamp;

		int powered;

		PA500_tlm_packet tlm_packet;


		PA500_status()
		{
			range.zero();

			device_status=0;
			timeStamp=0;

			powered = 0;

			tlm_packet.range=0;
			tlm_packet.device_status=0;
		}


		PA500_status(PA500_status &d)
		{
			copy_data(d);
		}


		~PA500_status(){}


		PA500_status& operator=(const PA500_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(PA500_tlm_packet &tp)
		{
			tp.range=(int64)(range.value*PA500_factor);
			tp.device_status=(int64)device_status;
		}
};


#endif /* PA500_STATUS_H_ */

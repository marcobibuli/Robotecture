/*
 * Echologger_status.h
 *
 *  Created on: Feb 05, 2019
 *      Author: mx
 */

#ifndef ECHOLOGGER_STATUS_H_
#define ECHOLOGGER_STATUS_H_

#include "../generic/custom_types.h"
#include "../generic/Variable.h"

# pragma pack (push, 1)
struct Echologger_tlm_packet
{
	int64 range;
	int64 device_status;
};
# pragma pack (pop)


class Echologger_status
{
	private:

		void copy_data(const Echologger_status &d)
		{
			range=d.range;

			device_status=d.device_status;
			timeStamp=d.timeStamp;

			compose_tlm_packet(tlm_packet);
		}


	public:

		Variable range;

		int device_status;
		int64 timeStamp;

		Echologger_tlm_packet tlm_packet;


		Echologger_status()
		{
			range.zero();

			device_status=0;
			timeStamp=0;

			tlm_packet.range=0;
			tlm_packet.device_status=0;
		}


		Echologger_status(Echologger_status &d)
		{
			copy_data(d);
		}


		~Echologger_status(){}


		Echologger_status& operator=(const Echologger_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(Echologger_tlm_packet &tp)
		{
			tp.range=(int64)(range.value*Echologger_factor);
			tp.device_status=(int64)device_status;
		}
};


#endif /* ECHOLOGGER_STATUS_H_ */

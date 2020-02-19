/*
 * DVL_status.h
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */

#ifndef DVL_STATUS_H_
#define DVL_STATUS_H_

#include "../generic/custom_types.h"
#include "../generic/Variable.h"


# pragma pack (push, 1)
struct DVL_tlm_packet
{
	int64 range1;
	int64 range2;
	int64 range3;
	int64 range4;
	int64 altitude;
	int64 surgeVelocity;
	int64 swayVelocity;
	int64 heaveVelocity;
	int64 errorVelocity;
	int64 validVelocity;
	int64 device_status;
};
# pragma pack (pop)


class DVL_status
{
	private:

		void copy_data(const DVL_status &d)
		{
			range1=d.range1; range2=d.range2; range3=d.range3; range4=d.range4;
			altitude=d.altitude;
			surgeVelocity=d.surgeVelocity;
			swayVelocity=d.swayVelocity;
			heaveVelocity=d.heaveVelocity;
			errorVelocity=d.errorVelocity;
			validVelocity=d.validVelocity;

			device_status=d.device_status;
			timeStamp=d.timeStamp;

			compose_tlm_packet(tlm_packet);
		}


	public:

		Variable range1,range2,range3,range4,altitude;
		Variable surgeVelocity,swayVelocity,heaveVelocity,errorVelocity;
		int validVelocity;

		int device_status;
		int64 timeStamp;

		DVL_tlm_packet tlm_packet;

		DVL_status()
		{
			range1.zero(); range2.zero(); range3.zero(); range4.zero();
			altitude.zero();
			surgeVelocity.zero();
			swayVelocity.zero();
			heaveVelocity.zero();
			errorVelocity.zero();
			validVelocity=0;

			device_status=0;
			timeStamp=0;

			tlm_packet.range1=0; tlm_packet.range2=0; tlm_packet.range3=0; tlm_packet.range4=0;
			tlm_packet.altitude=0;
			tlm_packet.surgeVelocity=0;
			tlm_packet.swayVelocity=0;
			tlm_packet.heaveVelocity=0;
			tlm_packet.errorVelocity=0;
			tlm_packet.validVelocity=0;
			tlm_packet.device_status=0;
		}


		DVL_status(DVL_status &d)
		{
			copy_data(d);
		}


		~DVL_status(){}


		DVL_status& operator=(const DVL_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(DVL_tlm_packet &tp)
		{
			tp.range1=(int64)((range1.value)*DVL_factor);
			tp.range2=(int64)((range2.value)*DVL_factor);
			tp.range3=(int64)((range3.value)*DVL_factor);
			tp.range4=(int64)((range4.value)*DVL_factor);
			tp.altitude=(int64)((altitude.value)*DVL_factor);
			tp.surgeVelocity=(int64)((surgeVelocity.value)*DVL_factor);
			tp.swayVelocity=(int64)((swayVelocity.value)*DVL_factor);
			tp.heaveVelocity=(int64)((heaveVelocity.value)*DVL_factor);
			tp.errorVelocity=(int64)((errorVelocity.value)*DVL_factor);
			tp.validVelocity=(int64)(validVelocity);
			tp.device_status=(int64)(device_status);
		}
};


#endif /* DVL_STATUS_H_ */

/*
 * IO_europe_status.h
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */

#ifndef IO_EUROPE_STATUS_H_
#define IO_EUROPE_STATUS_H_

#include <math.h>
#include "../generic/custom_types.h"
#include "../generic/Variable.h"
#include "../drivers/europe/hardwareLib/definitions.h"


#define EUROPE_DIO_MAX_CHANNELS (48)
#define EUROPE_AD_MAX_CHANNELS  (32)
#define EUROPE_DA_MAX_CHANNELS   (8)

/*
typedef enum
{
	EUROPE_DIO_MOTOR_VFL_POWER = 0, // DIO-00-B0-A0 - OUTPUT
	EUROPE_DIO_MOTOR_VFR_POWER = 1, // DIO-01-B0-A1 - OUTPUT
	EUROPE_DIO_MOTOR_VRL_POWER = 2, // DIO-02-B0-A2 - OUTPUT
	EUROPE_DIO_MOTOR_VRR_POWER = 3, // DIO-03-B0-A3 - OUTPUT
	EUROPE_DIO_MOTOR_HFL_POWER = 4, // DIO-04-B0-A4 - OUTPUT
	EUROPE_DIO_MOTOR_HFR_POWER = 5, // DIO-05-B0-A5 - OUTPUT
	EUROPE_DIO_MOTOR_HRL_POWER = 6, // DIO-06-B0-A6 - OUTPUT
	EUROPE_DIO_MOTOR_HRR_POWER = 7, // DIO-07-B0-A7 - OUTPUT
	EUROPE_DIO_MOTOR_VFL_ENABLE = 8, // DIO-08-B0-B0 - OUTPUT
	EUROPE_DIO_MOTOR_VFR_ENABLE = 9, // DIO-08-B0-B1 - OUTPUT
	EUROPE_DIO_MOTOR_VRL_ENABLE = 10, // DIO-10-B0-B2 - OUTPUT
	EUROPE_DIO_MOTOR_VRR_ENABLE = 11, // DIO-11-B0-B3 - OUTPUT
	EUROPE_DIO_MOTOR_HFL_ENABLE = 12, // DIO-12-B0-B4 - OUTPUT
	EUROPE_DIO_MOTOR_HFR_ENABLE = 13, // DIO-13-B0-B5 - OUTPUT
	EUROPE_DIO_MOTOR_HRL_ENABLE = 14, // DIO-14-B0-B6 - OUTPUT
	EUROPE_DIO_MOTOR_HRR_ENABLE = 15, // DIO-15-B0-B7 - OUTPUT
	EUROPE_DIO_LIGHTS_UP_POWER = 16, // DIO-16-B0-C0 - OUTPUT
	EUROPE_DIO_LIGHTS_MIDDLE_POWER = 17, // DIO-17-B0-C1 - OUTPUT
	EUROPE_DIO_LIGHTS_DOWN_POWER = 18, // DIO-18-B0-C2 - OUTPUT
	EUROPE_DIO_PA500_1_POWER = 19, // DIO-19-B0-C3 - OUTPUT
	EUROPE_DIO_PA500_2_POWER = 20, // DIO-20-B0-C4 - OUTPUT
	EUROPE_DIO_FIBER_OPTIC_GYRO_POWER = 21, // DIO-21-B0-C5 - OUTPUT
	EUROPE_DIO_EVOLOGICS_MODEM_POWER = 22, // DIO-22-B0-C6 - OUTPUT
	EUROPE_DIO_DVL_POWER = 23, // DIO-23-B0-C7 - OUTPUT
	EUROPE_DIO_NEWCASTLE_MODEM_POWER = 24, // DIO-24-B1-A0 - OUTPUT
	EUROPE_DIO_CPU_2_POWER = 25, // DIO-25-B1-A1 - OUTPUT
	EUROPE_DIO_POE_POWER = 26, // DIO-26-B1-A2 - OUTPUT
	EUROPE_DIO_AVAILABLE_27 = 27, // DIO-27-B1-A3 - OUTPUT
	EUROPE_DIO_AVAILABLE_28 = 28, // DIO-28-B1-A4 - OUTPUT
	EUROPE_DIO_AVAILABLE_29 = 29, // DIO-29-B1-A5 - OUTPUT
	EUROPE_DIO_AVAILABLE_30 = 30, // DIO-30-B1-A6 - OUTPUT
	EUROPE_DIO_AVAILABLE_31 = 31, // DIO-31-B1-A7 - OUTPUT
	EUROPE_DIO_MOTOR_VFL_FAULT = 32, // DIO-32-B1-B0 - INPUT
	EUROPE_DIO_MOTOR_VFR_FAULT = 33, // DIO-33-B1-B1 - INPUT
	EUROPE_DIO_MOTOR_VRL_FAULT = 34, // DIO-34-B1-B2 - INPUT
	EUROPE_DIO_MOTOR_VRR_FAULT = 35, // DIO-35-B1-B3 - INPUT
	EUROPE_DIO_MOTOR_HFL_FAULT = 36, // DIO-36-B1-B4 - INPUT
	EUROPE_DIO_MOTOR_HFR_FAULT = 37, // DIO-37-B1-B5 - INPUT
	EUROPE_DIO_MOTOR_HRL_FAULT = 38, // DIO-38-B1-B6 - INPUT
	EUROPE_DIO_MOTOR_HRR_FAULT = 39, // DIO-39-B1-B7 - INPUT
	EUROPE_DIO_AVAILABLE_40 = 40, // DIO-40-B1-C0 - INPUT
	EUROPE_DIO_AVAILABLE_41 = 41, // DIO-41-B1-C1 - INPUT
	EUROPE_DIO_AVAILABLE_42 = 42, // DIO-42-B1-C2 - INPUT
	EUROPE_DIO_AVAILABLE_43 = 43, // DIO-43-B1-C3 - INPUT
	EUROPE_DIO_AVAILABLE_44 = 44, // DIO-44-B1-C4 - OUTPUT
	EUROPE_DIO_AVAILABLE_45 = 45, // DIO-45-B1-C5 - OUTPUT
	EUROPE_DIO_AVAILABLE_46 = 46, // DIO-46-B1-C6 - OUTPUT
	EUROPE_DIO_AVAILABLE_47 = 47  // DIO-47-B1-C7 - OUTPUT
}EuropeDioEnum;
*/
/*
typedef enum
{
	EUROPE_DA_MOTOR_VFL_VREF = 0, // AO-00-B0-V00 - OUTPUT
	EUROPE_DA_MOTOR_VFR_VREF = 1, // AO-01-B0-V01 - OUTPUT
	EUROPE_DA_MOTOR_VRL_VREF = 2, // AO-02-B0-V02 - OUTPUT
	EUROPE_DA_MOTOR_VRR_VREF = 3, // AO-03-B0-V03 - OUTPUT
	EUROPE_DA_MOTOR_HFL_VREF = 4, // AO-04-B1-V00 - OUTPUT
	EUROPE_DA_MOTOR_HFR_VREF = 5, // AO-05-B1-V01 - OUTPUT
	EUROPE_DA_MOTOR_HRL_VREF = 6, // AO-06-B1-V02 - OUTPUT
	EUROPE_DA_MOTOR_HRR_VREF = 7  // AO-07-B1-V03 - OUTPUT
}EuropeDAEnum;
*/

enum MOTOR_STATUS
{
	MOTOR_OFF = 0,
	MOTOR_POWERED = 1,
	MOTOR_ENABLED = 2,
	MOTOR_FAULT = 3
};


enum IO_cmd
{
	SET_SIM = 0,
	SET_ACT = 1,
	SET_DIGITAL = 2,
	SET_ANALOG = 3,
	AUTO_START = 4,
	AUTO_STOP = 5
};



# pragma pack (push, 1)
struct IO_europe_tlm_packet
{
	int64 analogInput[EUROPE_AD_MAX_CHANNELS];
	int64 analogOutput[EUROPE_DA_MAX_CHANNELS];
	int64 digital;
	int64 device_status;
};
# pragma pack (pop)




# pragma pack (push, 1)
struct IO_europe_sim_cmd_packet
{
	int64 da[EUROPE_DA_MAX_CHANNELS];
	int64 digital;
};
# pragma pack (pop)



# pragma pack (push, 1)
struct IO_europe_cmd_packet
{
	int32 cmd_type;
	int32 index;
	int64 value;
};
# pragma pack (pop)




class IO_europe_status
{
	private:

		void copy_data(const IO_europe_status&d)
		{
			for (int i = 0; i < EUROPE_DIO_MAX_CHANNELS; i++)
			{
				digitalInput[i] = d.digitalInput[i];
				digitalOutput[i] = d.digitalOutput[i];
			}

			for (int i = 0; i < EUROPE_AD_MAX_CHANNELS; i++)
				analogInput[i] = d.analogInput[i];

			for (int i = 0; i < EUROPE_DA_MAX_CHANNELS; i++)
				analogOutput[i] = d.analogOutput[i];
			
			device_status =d.device_status;
			timeStamp=d.timeStamp;

			updated = d.updated;

			compose_tlm_packet(tlm_packet);
		}


	public:

		double analogInput[EUROPE_AD_MAX_CHANNELS];
		double analogOutput[EUROPE_DA_MAX_CHANNELS];
		int digitalInput[EUROPE_DIO_MAX_CHANNELS];
		int digitalOutput[EUROPE_DIO_MAX_CHANNELS];
		
		int64 device_status;
		int64 timeStamp;

		bool updated;

		IO_europe_tlm_packet tlm_packet;

		IO_europe_status()
		{
			for (int i = 0; i < EUROPE_DIO_MAX_CHANNELS; i++)
			{
				digitalInput[i] = 0;
				digitalOutput[i] = 0;
			}
			tlm_packet.digital = 0;
			

			for (int i = 0; i < EUROPE_AD_MAX_CHANNELS; i++)
			{
				analogInput[i] = 0.0;
				tlm_packet.analogInput[i] = 0;
			}

			for (int i = 0; i < EUROPE_DA_MAX_CHANNELS; i++)
			{
				analogOutput[i] = 0.0;
				tlm_packet.analogOutput[i] = 0;
			}

			device_status =0;
			timeStamp=0;

			updated = false;
		}


		IO_europe_status(IO_europe_status&d)
		{
			copy_data(d);
		}


		~IO_europe_status(){}


		IO_europe_status& operator=(const IO_europe_status&d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(IO_europe_tlm_packet&tp)
		{
			tp.digital = 0;
			for (int i = 0; i < EUROPE_DIO_MAX_CHANNELS; i++)
			{
				tp.digital= (tp.digital<<1) + digitalInput[EUROPE_DIO_MAX_CHANNELS-1-i] ;
			}
			

			for (int i = 0; i < EUROPE_AD_MAX_CHANNELS; i++)
				tp.analogInput[i] = (int64)(analogInput[i]*IO_factor);

			for (int i = 0; i < EUROPE_DA_MAX_CHANNELS; i++)
				tp.analogOutput[i] = (int64)(analogOutput[i] * IO_factor);
			
			tp.device_status =(int64)(device_status);
		}


		void extract_tlm_packet(IO_europe_tlm_packet tp)
		{
			for (int i = 0; i < EUROPE_DIO_MAX_CHANNELS; i++)
			{
				digitalInput[i] = tp.digital & 1;
				tp.digital = tp.digital >> 1;
			}
				

			for (int i = 0; i < EUROPE_AD_MAX_CHANNELS; i++)
				analogInput[i] = ((double)tp.analogInput[i]) / IO_factor;

			for (int i = 0; i < EUROPE_DA_MAX_CHANNELS; i++)
				analogOutput[i] = ((double)tp.analogOutput[i]) / IO_factor;

			device_status = tp.device_status;
		}
};


#endif /* IO_EUROPE_STATUS_H_ */

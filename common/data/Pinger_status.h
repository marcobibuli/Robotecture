/*
 * Pinger_status.h
 *
 *  Created on: Jan 31, 2019
 *      Author: mx
 */

#ifndef PINGER_STATUS_H_
#define PINGER_STATUS_H_

#include "../generic/define.h"
#include "../generic/custom_types.h"
#include "../generic/Variable.h"
#include <vector>
#include <stdio.h>
#include <string>


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
			
			//printf(" d.acoustic_tlm: %s\n", d.acoustic_tlm);

			//strcpy(acoustic_tlm, d.acoustic_tlm);
			acoustic_tlm = d.acoustic_tlm;

			/*
			for (int i = 0; i < acoustic_cmd.size(); i++)
				delete acoustic_cmd[i];
			

			acoustic_cmd.clear();

			for (int i = 0; i < d.acoustic_cmd.size(); i++)
			{
				char *s=new char[BUF_SIZE];
				strcpy(s, d.acoustic_cmd[i]);
				acoustic_cmd.push_back(s);
			}
			*/

			acoustic_cmd = d.acoustic_cmd;


			compose_tlm_packet(tlm_packet);
		}


	public:

		Variable x,y,z;

		int device_status;
		int64 timeStamp;

		int powered;

		
		//char acoustic_tlm[BUF_SIZE];

		//std::vector<char*> acoustic_cmd;
		//char* acoustic_tlm;

		std::vector<std::string> acoustic_cmd;
		std::string acoustic_tlm;

		Pinger_tlm_packet tlm_packet;

		Pinger_status()
		{
			x.zero();
			y.zero();
			z.zero();

			device_status=0;
			timeStamp=0;

			//acoustic_tlm = new char[BUF_SIZE];
			//sprintf(acoustic_tlm, "0");

			acoustic_tlm = "0";

			acoustic_cmd.clear();

			powered = 0;

			tlm_packet.x=0; tlm_packet.y=0; tlm_packet.z=0;
			tlm_packet.device_status=0;
		}


		Pinger_status(Pinger_status&d)
		{
			copy_data(d);
		}


		~Pinger_status()
		{
			/*
			delete []acoustic_tlm;

			for (int i = 0; i < acoustic_cmd.size(); i++)
				delete acoustic_cmd[i];
			*/
			acoustic_cmd.clear();
			
		}


		void add_cmd(char* cmd)
		{
			//char* s = new char[BUF_SIZE];
			//strcpy(s, cmd);
			std::string s = cmd;
			acoustic_cmd.push_back(s);
		}


		Pinger_status& operator=(const Pinger_status&d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(Pinger_tlm_packet &tp)
		{
			tp.x=(int64)(x.value* USBLpos_factor);
			tp.y=(int64)(y.value* USBLpos_factor);
			tp.z=(int64)(z.value* USBLpos_factor);
			tp.device_status=(int64)device_status;
		}

		void compose_string_packet(char* str)
		{
			sprintf(str, "%lli", device_status);
		}
};


#endif /* PINGER_STATUS_H_ */

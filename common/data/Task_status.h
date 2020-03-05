/*
 * Task_status.h
 *
 *  Created on: Feb 11, 2019
 *      Author: mx
 */

#ifndef DATA_TASK_STATUS_H_
#define DATA_TASK_STATUS_H_

#include "../generic/custom_types.h"



enum TaskStatus
{
	TASK_OFF=0,
	TASK_INIT=2,
	TASK_RUNNING=3,
	TASK_WARNING=4,
	TASK_FAULT=5
};

# pragma pack (push, 1)
struct Task_tlm_packet
{
	int64 execution;

	void zero()
	{
		execution=0;
	}
};
# pragma pack (pop)

class Task_status
{
	private:

		void copy_data(const Task_status &d)
		{
			execution=d.execution;

			compose_tlm_packet(tlm_packet);
		}


	public:

		TaskStatus execution;

		Task_tlm_packet tlm_packet;


		Task_status()
		{
			execution=TASK_OFF;
			tlm_packet.zero();
		}


		Task_status(Task_status &d)
		{
			copy_data(d);
		}


		~Task_status(){}


		Task_status& operator=(const Task_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(Task_tlm_packet &tp)
		{
			tp.execution=(int64)execution;
		}
};


#endif /* DATA_TASK_STATUS_H_ */

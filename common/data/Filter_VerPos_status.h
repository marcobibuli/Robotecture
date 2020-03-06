/*
 * Filter_VerPos_status.h
 *
 *  Created on: Feb 11, 2019
 *      Author: mx
 */

#ifndef DATA_FILTER_VERPOS_STATUS_H_
#define DATA_FILTER_VERPOS_STATUS_H_


#include "../data/Task_status.h"

struct Filter_VerPos_tlm_packet
{
	int64 q_z;
	int64 r_z;
	int64 execution;

	void zero()
	{
		execution=0;
		q_z=0;
		r_z=0;
	}
};


class Filter_VerPos_status
{
	private:



		void copy_data(const Filter_VerPos_status &d)
		{
			execution=d.execution;
			q_z=d.q_z;
			r_z=d.r_z;
		}


	public:

		TaskStatus execution;
		double q_z;
		double r_z;


		Filter_VerPos_status()
		{
			execution=TASK_INIT;
			q_z=0.0;
			r_z=0.0;
		}


		Filter_VerPos_status(Filter_VerPos_status &d)
		{
			copy_data(d);
		}


		~Filter_VerPos_status(){}


		Filter_VerPos_status& operator=(const Filter_VerPos_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(Filter_VerPos_tlm_packet &tp)
		{
			tp.execution=execution;
			tp.q_z=(int64)(q_z*NGC_factor);
			tp.r_z=(int64)(r_z*NGC_factor);
		}
};


#endif /* DATA_FILTER_VERPOS_STATUS_H_ */

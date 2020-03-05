/*
 * Filter_HorPos_status.h
 *
 *  Created on: Feb 11, 2019
 *      Author: mx
 */

#ifndef DATA_FILTER_HORPOS_STATUS_H_
#define DATA_FILTER_HORPOS_STATUS_H_


#include "../data/Task_status.h"

struct Filter_HorPos_tlm_packet
{
	int64 q_xy;
	int64 r_xy_11;
	int64 r_xy_12;
	int64 r_xy_22;
	int64 execution;

	void zero()
	{
		execution=0;
		q_xy=0;
		r_xy_11=0;
		r_xy_12=0;
		r_xy_22=0;
	}
};


class Filter_HorPos_status
{
	private:



		void copy_data(const Filter_HorPos_status &d)
		{
			execution=d.execution;
			q_xy=d.q_xy;
			r_xy_11=d.r_xy_11;
			r_xy_12=d.r_xy_12;
			r_xy_22=d.r_xy_22;
		}


	public:

		TaskStatus execution;
		double q_xy;
		double r_xy_11,r_xy_12,r_xy_22;


		Filter_HorPos_status()
		{
			execution=TASK_INIT;
			q_xy=0.0;
			r_xy_11=0.0;
			r_xy_12=0.0;
			r_xy_22=0.0;
		}


		Filter_HorPos_status(Filter_HorPos_status &d)
		{
			copy_data(d);
		}


		~Filter_HorPos_status(){}


		Filter_HorPos_status& operator=(const Filter_HorPos_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(Filter_HorPos_tlm_packet &tp)
		{
			tp.execution=execution;
			tp.q_xy=(int64)(q_xy*NGC_factor);
			tp.r_xy_11=(int64)(r_xy_11*NGC_factor);
			tp.r_xy_12=(int64)(r_xy_12*NGC_factor);
			tp.r_xy_22=(int64)(r_xy_22*NGC_factor);
		}
};


#endif /* DATA_FILTER_HORPOS_STATUS_H_ */

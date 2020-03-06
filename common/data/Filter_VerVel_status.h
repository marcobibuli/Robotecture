/*
 * Filter_VerVel_status.h
 *
 *  Created on: Feb 11, 2019
 *      Author: mx
 */

#ifndef DATA_FILTER_VERVEL_STATUS_H_
#define DATA_FILTER_VERVEL_STATUS_H_


#include "../data/Task_status.h"

struct Filter_VerVel_tlm_packet
{
	int64 q_w;
	int64 r_w;
	int64 Kw;
	int64 bw;
	int64 gw;
	int64 execution;

	void zero()
	{
		execution=0;
		q_w=0;
		r_w=0;
		Kw=0;
		bw=0;
		gw=0;
	}
};


class Filter_VerVel_status
{
	private:



		void copy_data(const Filter_VerVel_status &d)
		{
			execution=d.execution;
			q_w=d.q_w;
			r_w=d.r_w;
			Kw=d.Kw;
			bw=d.bw;
			gw=d.gw;
		}


	public:

		TaskStatus execution;
		double q_w;
		double r_w;
		double Kw;
		double bw;
		double gw;


		Filter_VerVel_status()
		{
			execution=TASK_INIT;
			q_w=0.0;
			r_w=0.0;
			Kw=0.0;
			bw=0.0;
			gw=0.0;
		}


		Filter_VerVel_status(Filter_VerVel_status &d)
		{
			copy_data(d);
		}


		~Filter_VerVel_status(){}


		Filter_VerVel_status& operator=(const Filter_VerVel_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(Filter_VerVel_tlm_packet &tp)
		{
			tp.execution=execution;
			tp.q_w=(int64)(q_w*NGC_factor);
			tp.r_w=(int64)(r_w*NGC_factor);
			tp.Kw=(int64)(Kw*NGC_factor);
			tp.bw=(int64)(bw*NGC_factor);
			tp.gw=(int64)(gw*NGC_factor);
		}
};


#endif /* DATA_FILTER_VERVEL_STATUS_H_ */

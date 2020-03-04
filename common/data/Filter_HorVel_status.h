/*
 * Filter_HorVel_status.h
 *
 *  Created on: Feb 11, 2019
 *      Author: mx
 */

#ifndef DATA_FILTER_HORVEL_STATUS_H_
#define DATA_FILTER_HORVEL_STATUS_H_


#include "../data/Task_status.h"

# pragma pack (push, 1)
struct Filter_HorVel_tlm_packet
{
	int64 q_uv;
	int64 q_current;
	int64 r_uv_11;
	int64 r_uv_12;
	int64 r_uv_22;
	int64 Ku;
	int64 bu;
	int64 Kv;
	int64 bv;
	int64 execution;

	void zero()
	{
		execution=0;
		q_uv=0; q_current=0;
		r_uv_11=0; r_uv_12=0; r_uv_22=0;
		Ku=0; bu=0; Kv=0; bv=0;
	}
};
# pragma pack (pop)


class Filter_HorVel_status
{
	private:



		void copy_data(const Filter_HorVel_status &d)
		{
			execution=d.execution;
			q_uv=d.q_uv;
			q_current=d.q_current;
			r_uv_11=d.r_uv_11;
			r_uv_12=d.r_uv_12;
			r_uv_22=d.r_uv_22;
			Ku=d.Ku;
			bu=d.bu;
			Kv=d.Kv;
			bv=d.bv;
		}


	public:

		TaskStatus execution;
		double q_uv;
		double q_current;
		double r_uv_11;
		double r_uv_12;
		double r_uv_22;
		double Ku;
		double bu;
		double Kv;
		double bv;


		Filter_HorVel_status()
		{
			execution=TASK_INIT;
			q_uv=0.0; q_current=0.0;
			r_uv_11=0.0; r_uv_12=0.0; r_uv_22=0.0;
			Ku=0.0; bu=0.0; Kv=0.0; bv=0.0;
		}


		Filter_HorVel_status(Filter_HorVel_status &d)
		{
			copy_data(d);
		}


		~Filter_HorVel_status(){}


		Filter_HorVel_status& operator=(const Filter_HorVel_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(Filter_HorVel_tlm_packet &tp)
		{
			tp.execution=execution;
			tp.q_uv=(int64)(q_uv*NGC_factor);
			tp.q_current=(int64)(q_current*NGC_factor);
			tp.r_uv_11=(int64)(r_uv_11*NGC_factor);
			tp.r_uv_12=(int64)(r_uv_12*NGC_factor);
			tp.r_uv_22=(int64)(r_uv_22*NGC_factor);
			tp.Ku=(int64)(Ku*NGC_factor);
			tp.bu=(int64)(bu*NGC_factor);
			tp.Kv=(int64)(Kv*NGC_factor);
			tp.bv=(int64)(bv*NGC_factor);
		}
};


#endif /* DATA_FILTER_HORVEL_STATUS_H_ */

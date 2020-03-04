/*
 * Filer_Ang_status.h
 *
 *  Created on: Feb 11, 2019
 *      Author: mx
 */

#ifndef DATA_FILTER_ANG_STATUS_H_
#define DATA_FILTER_ANG_STATUS_H_

#include "../data/Task_status.h"

# pragma pack (push, 1)
struct Filter_Ang_tlm_packet
{
	int64 q_psi;
	int64 q_r;
	int64 r_psi;
	int64 r_r;
	int64 Kr;
	int64 br;
	int64 execution;

	void zero()
	{
		execution=0;
		q_psi=0; q_r=0;
		r_psi=0; r_r=0;
		Kr=0; br=0;
	}
};
# pragma pack (pop)

class Filter_Ang_status
{
	private:



		void copy_data(const Filter_Ang_status &d)
		{
			execution=d.execution;
			q_psi=d.q_psi;
			q_r=d.q_r;
			r_psi=d.r_psi;
			r_r=d.r_r;
			Kr=d.Kr;
			br=d.br;
		}


	public:

		TaskStatus execution;
		double q_psi,q_r,r_psi,r_r,Kr,br;


		Filter_Ang_status()
		{
			execution=TASK_INIT;
			q_psi=0.0; q_r=0.0;
			r_psi=0.0; r_r=0.0;
			Kr=0.0; br=0.0;
		}


		Filter_Ang_status(Filter_Ang_status &d)
		{
			copy_data(d);
		}


		~Filter_Ang_status(){}


		Filter_Ang_status& operator=(const Filter_Ang_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(Filter_Ang_tlm_packet &tp)
		{
			tp.execution=execution;
			tp.q_psi=(int64)(q_psi*NGC_factor);
			tp.q_r=(int64)(q_r*NGC_factor);
			tp.r_psi=(int64)(r_psi*NGC_factor);
			tp.r_r=(int64)(r_r*NGC_factor);
			tp.Kr=(int64)(Kr*NGC_factor);
			tp.br=(int64)(br*NGC_factor);
		}
};


#endif /* DATA_FILTER_ANG_STATUS_H_ */

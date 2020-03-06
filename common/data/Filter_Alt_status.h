/*
 * Filter_Alt_status.h
 *
 *  Created on: Feb 11, 2019
 *      Author: mx
 */

#ifndef DATA_FILTER_ALT_STATUS_H_
#define DATA_FILTER_ALT_STATUS_H_

#include "../data/Task_status.h"

struct Filter_Alt_tlm_packet
{
	int64 q_alt;
	int64 r_alt;
	int64 execution;

	void zero()
	{
		execution=0;
		q_alt=0;
		r_alt=0;
	}
};


class Filter_Alt_status
{
	private:



		void copy_data(const Filter_Alt_status &d)
		{
			execution=d.execution;
			q_alt=d.q_alt;
			r_alt=d.r_alt;

		}


	public:

		TaskStatus execution;
		double q_alt;
		double r_alt;


		Filter_Alt_status()
		{
			execution=TASK_INIT;
			q_alt=0.0;
			r_alt=0.0;
		}


		Filter_Alt_status(Filter_Alt_status &d)
		{
			copy_data(d);
		}


		~Filter_Alt_status(){}


		Filter_Alt_status& operator=(const Filter_Alt_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(Filter_Alt_tlm_packet &tp)
		{
			tp.execution=execution;
			tp.q_alt=(int64)(q_alt*NGC_factor);
			tp.r_alt=(int64)(r_alt*NGC_factor);
		}
};



#endif /* DATA_FILTER_ALT_STATUS_H_ */

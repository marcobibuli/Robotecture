/*
 * Time_status.h
 *
 *  Created on: Jan 21, 2019
 *      Author: mx
 */

#ifndef TIME_STATUS_H_
#define TIME_STATUS_H_

#include "../generic/custom_types.h"



class Time_status
{
	private:

		void copy_data(const Time_status &d)
		{
			timeStamp=d.timeStamp;
			dt=d.dt;
			year = d.year;	month = d.month;	day = d.day;
			hour = d.hour;	min = d.min;		sec = d.sec;	msec = d.msec;
		}


	public:

		uint64 timeStamp;
		double dt;
		
		int32 year, month, day, hour, min, sec, msec;


		Time_status()
		{
			timeStamp=0;
			dt=0.0;
			year = 0;	month = 0;	day = 0;
			hour = 0;	min = 0;	sec = 0;	msec = 0;
		}


		Time_status(Time_status &d)
		{
			copy_data(d);
		}


		~Time_status(){}


		Time_status& operator=(const Time_status &d)
		{
			copy_data(d);
			return *this;
		}


};


#endif /* TIME_STATUS_H_ */

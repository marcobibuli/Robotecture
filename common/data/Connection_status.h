/*
 * Connection_status.h
 *
 *  Created on: Feb 24, 2020
 *      Author: mx
 */

#ifndef CONNECTION_STATUS_H_
#define CONNECTION_STATUS_H_

#include "../generic/custom_types.h"



class Connection_status
{
	private:

		void copy_data(const Connection_status& d)
		{
			timeStamp=d.timeStamp;
			ethernet_active =d.ethernet_active;
			acoustics_active = d.acoustics_active;
		}


	public:

		bool ethernet_active;
		bool acoustics_active;
		uint64 timeStamp;


		Connection_status()
		{
			timeStamp=0;
			ethernet_active = false;
			acoustics_active = false;
		}


		Connection_status(Connection_status&d)
		{
			copy_data(d);
		}


		~Connection_status(){}


		Connection_status& operator=(const Connection_status&d)
		{
			copy_data(d);
			return *this;
		}


};


#endif /* TIME_STATUS_H_ */

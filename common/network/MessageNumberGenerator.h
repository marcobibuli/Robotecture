/*
 * MessageNumberGenerator.h
 *
 *  Created on: Nov 9, 2016
 *      Author: mx
 */

#ifndef NETWORK_MESSAGENUMBERGENERATOR_H_
#define NETWORK_MESSAGENUMBERGENERATOR_H_

#include <limits.h>

class MessageNumberGenerator
{
	private:
		int64 num;
		int64 max_num;

	public:
		MessageNumberGenerator() { num=0; max_num=LLONG_MAX-1;}
		~MessageNumberGenerator() {  }

		int64 get()
		{
			int64 r=num;
			if (num==max_num) num=0;
			else num++;
			return r;
		}

		int64 get_max() { return max_num; }
};


#endif /* NETWORK_MESSAGENUMBERGENERATOR_H_ */

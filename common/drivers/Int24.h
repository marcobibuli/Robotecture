/*
 * Int24.h
 *
 *  Created on: May 28, 2015
 *      Author: enrica
 */

#ifndef INT24_H_
#define INT24_H_

#include <stdint.h>

const int INT24_MAX = 8388607;

class Int24
{
protected:
	unsigned char value[3];
public:
	Int24(){}

	Int24( const Int24& val )
	{
		*this   = val;
	}

	void swap()
	{
		unsigned char dummy;
		dummy = value[2];
		value[2] = value[0];
		value[0] = dummy;
	}

	operator int32_t() const
	{
		/* Sign extend negative quantities */
		if( value[2] & 0x80 ) {
			return (0xff    << 24)
				| (value[2] << 16)
				| (value[1] <<  8)
				|  value[0];
		} else {
			return (value[2] << 16)
				| (value[1] <<  8)
				|  value[0];
		}
	}

	Int24& operator= (const Int24& input)
	{
		value[0]   = input.value[0];
		value[1]   = input.value[1];
		value[2]   = input.value[2];

		return *this;
	}

	Int24& operator= (const int input)
	{
		value[0]   = ((unsigned char*)&input)[0];
		value[1]   = ((unsigned char*)&input)[1];
		value[2]   = ((unsigned char*)&input)[2];

		return *this;
	}

	Int24& operator= (const unsigned char* input)
	{
		value[0]   = input[0];
		value[1]   = input[1];
		value[2]   = input[2];

		return *this;
	}
};



#endif /* INT24_H_ */

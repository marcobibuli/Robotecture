/*
 * Command_status.h
 *
 *  Created on: Mar 05, 2020
 *      Author: mx
 */

#ifndef COMMAND_STATUS_H_
#define COMMAND_STATUS_H_

#include "../generic/custom_types.h"
#include <vector>
#include <string>



class Command_status
{
	private:

		void copy_data(const Command_status&d)
		{
			commands = d.commands;
		}


	public:

		std::vector<std::string> commands;


		Command_status()
		{
			commands.clear();	
		}


		Command_status(Command_status&d)
		{
			copy_data(d);
		}


		~Command_status(){}


		Command_status& operator=(const Command_status&d)
		{
			copy_data(d);
			return *this;
		}

};


#endif /* COMMAND_STATUS_H_ */

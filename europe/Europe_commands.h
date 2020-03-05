/*
 * Europe_commands.h
 *
 *  Created on: Feb 05, 2020
 *      Author: mx
 */

#ifndef EUROPE_COMMANDS_H_
#define EUROPE_COMMANDS_H_




#include "../common/generic/RobotThread.h"
#include "Europe_status.h"
#include "Tasks.h"
#include "../common/network/CommLink.h"
#include "../common/network/NetworkManager.h"
#include <string>
#include "../common/generic/util.h"

enum System
{
	ACOUSTIC_PING = 0,
	IO_SYSTEM = 1,
	NGC_SYSTEM = 2,
	TASK_SYSTEM = 3
};


class Europe_commands :public RobotThread
{
	private:
		void read_cmd(std::string cmd_string);
		void dispatch_cmd(std::vector<std::string>& cmd_msg, std::string cmd_string);
		
		void ngc_command(std::vector<std::string>& cmd_msg);
		void task_command(std::vector<std::string>& cmd_msg);

	protected:
		timespec tDelay, tStart, tSleep;
		CommLink* io_cmd;
		CommLink* task_cmd;
		NetworkManager* networkManager;

	public:
		Europe_commands(const char* name, NetworkManager& nm, Europe_status* s);
		~Europe_commands();

		virtual void execute();

		Europe_status *status;
};

void* start_europe_commands(void* arg);





#endif /* EUROPE_COMMANDS_H_ */
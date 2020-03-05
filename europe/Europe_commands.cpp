/*
 * europe_commands.cpp
 *
 *  Created on: Feb 05, 2020
 *      Author: mx
 */

#include "Europe_commands.h"



Europe_commands::Europe_commands(const char* name, NetworkManager& nm, Europe_status* s):RobotThread(name,SCHED_FIFO, COMMANDS_THREAD_PRIORITY,start_europe_commands)
{
	networkManager = &nm;
	status = s;

	io_cmd = new CommLink("IO_commands", OVERRIDE);
	io_cmd->open(networkManager->ROBOT_IP, networkManager->IO_ROBOT_HMI_PORT_IN,
		         networkManager->HMI_IP, networkManager->IO_HMI_PORT_OUT);
	io_cmd->create();


	task_cmd = new CommLink("Task_commnads", OVERRIDE);
	task_cmd->open(networkManager->ROBOT_IP, networkManager->TASKS_ROBOT_HMI_PORT_IN,
		           networkManager->HMI_IP, networkManager->TASKS_HMI_PORT_OUT);
	task_cmd->create();
}


Europe_commands::~Europe_commands()
{
	io_cmd->terminate();
	delete io_cmd;

	task_cmd->terminate();
	delete task_cmd;
}



void Europe_commands::execute()
{
	tSleep.tv_sec=CORE_SAMPLE_TIME_SEC;
	tSleep.tv_nsec=CORE_SAMPLE_TIME_NSEC;

	int ret;
	char cmd_buf[BUF_SIZE];
	std::string cmd_string;
	

	Pinger_status pinger_status;
	IO_europe_status io_status;

	Command_status task_commands;
	

	while(true)
	{

		do {

			pinger_status = status->pinger_status.get();

			if (pinger_status.acoustic_cmd.size() > 0)
			{
				read_cmd(pinger_status.acoustic_cmd.front());
				pinger_status.acoustic_cmd.erase(pinger_status.acoustic_cmd.begin());
				status->pinger_status.set(pinger_status);
			}

		} while (pinger_status.acoustic_cmd.size() > 0);




		do {
			ret = io_cmd->recv_message(cmd_buf);

			if (ret > 0)
			{
				cmd_buf[ret] = NULL;
				cmd_string = cmd_buf;

				io_status = status->io_status.get();
				io_status.commands.push_back(cmd_string);
				status->io_status.set(io_status);
			}

		} while (ret > 0);


		do {
			ret = task_cmd->recv_message(cmd_buf);

			if (ret > 0)
			{
				cmd_buf[ret] = NULL;
				cmd_string = cmd_buf;

				task_commands = status->task_commands.get();
				task_commands.commands.push_back(cmd_string);
				status->task_commands.set(task_commands);
			}

		} while (ret > 0);


		nanosleep(&tSleep,NULL);
	}
}


void Europe_commands::read_cmd(std::string cmd_string)
{
	std::vector<std::string> cmd_msg;
	
	parse_cmd(cmd_msg , cmd_string);// , cmd_string);
	
	dispatch_cmd(cmd_msg , cmd_string);

	cmd_msg.clear();
}





void Europe_commands::dispatch_cmd(std::vector<std::string>& cmd_msg, std::string cmd_string)
{
	System system;
	
	sscanf(cmd_msg[0].c_str(), "%d", &system);

	IO_europe_status io_status;
	Command_status ngc_commands;
	Command_status task_commands;

	switch (system)
	{
		case ACOUSTIC_PING:  //printf("Acoustic ping\n");
					         break;

		case IO_SYSTEM:  io_status = status->io_status.get();
			             io_status.commands.push_back(cmd_string);
			             status->io_status.set(io_status);
			             break;

		case NGC_SYSTEM: ngc_commands = status->ngc_commands.get();
						 ngc_commands.commands.push_back(cmd_string);
			             status->ngc_commands.set(ngc_commands);
			             break;

		case TASK_SYSTEM: task_commands = status->task_commands.get();
			              task_commands.commands.push_back(cmd_string);
			              status->task_commands.set(task_commands);
			              break;
	}

}





void Europe_commands::ngc_command(std::vector<std::string>& cmd_msg)
{

}


void Europe_commands::task_command(std::vector<std::string>& cmd_msg)
{/*
	Task_cmd task_cmd;
	sscanf(cmd_msg[1], "%d", &task_cmd);

	switch (task_cmd)
	{

		case SET_ANG_RAW_FILTER: if (((TaskStatus)msg.value) == SENSOR_RAW)
								 {
									actualAngFromFilter->setStatus(TASK_OFF);
									actualAngFromRaw->setStatus(TASK_RUNNING);
								 }
								 else if (((TaskStatus)msg.value) == SENSOR_FILTER)
								 {
									actualAngFromRaw->setStatus(TASK_OFF);
									actualAngFromFilter->setStatus(TASK_RUNNING);
								 }
						         break;
								 */
}


void* start_europe_commands(void *arg)
{
	Europe_commands* pThread=(Europe_commands*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}



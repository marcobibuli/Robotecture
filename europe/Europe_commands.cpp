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

	io_cmd = new CommLink("Telemetry", OVERRIDE);
	io_cmd->open(networkManager->ROBOT_IP, networkManager->IO_ROBOT_HMI_PORT_IN,
		         networkManager->HMI_IP, networkManager->IO_HMI_PORT_OUT);
	io_cmd->create();
}


Europe_commands::~Europe_commands()
{
	io_cmd->terminate();
	delete io_cmd;
}



void Europe_commands::execute()
{
	tSleep.tv_sec=CORE_SAMPLE_TIME_SEC;
	tSleep.tv_nsec=CORE_SAMPLE_TIME_NSEC;

	int ret;
	char cmd_string[BUF_SIZE];

	

	Pinger_status pinger_status;
	

	while(true)
	{
		
		pinger_status = status->pinger_status.get();

		//printf("cmd size: %d\n", pinger_status.acoustic_cmd.size());
		
		for (int i = 0; i < pinger_status.acoustic_cmd.size(); i++)
		{
			printf("acoustic cmd: %s     cmd size: %d\n", pinger_status.acoustic_cmd[i],pinger_status.acoustic_cmd.size());
			exec_cmd(pinger_status.acoustic_cmd[i]);
			delete pinger_status.acoustic_cmd[i];
		}
		pinger_status.acoustic_cmd.clear();
		status->pinger_status.set(pinger_status);
		


		do {
			ret = io_cmd->recv_message(cmd_string);
			if (ret > 0)
			{
				char* cmd = new char[ret + 1];
				for (int i = 0; i < ret; i++)
					cmd[i] = cmd_string[i];
				cmd[ret] = NULL;
				
				exec_cmd(cmd);
				delete[]cmd;
			}
		} while (ret > 0);


		nanosleep(&tSleep,NULL);
	}
}


void Europe_commands::exec_cmd(char* cmd_string)
{
	std::vector<char*> cmd_msg;

	parse_cmd(cmd_msg, cmd_string);

	dispatch_cmd(cmd_msg);

	for (int i = 0; i < cmd_msg.size(); i++)
		delete cmd_msg[i];

	cmd_msg.clear();
}


void Europe_commands::parse_cmd(std::vector<char*> &cmd_msg, char* cmd_string)
{
	int i = 0;
	char* s = NULL;
	int k = 0;
	do
	{
		if (cmd_string[i] == ' ' || cmd_string[i] == NULL)
		{
			if (s != NULL)
			{
				s[k] = NULL;
				cmd_msg.push_back(s);
				s = NULL;
			}
		}
		else
		{
			if (s == NULL)
			{
				s = new char[BUF_SIZE];
				k = 0;
			}

			s[k] = cmd_string[i];
			k++;
		}
	} while (cmd_string[i++] != NULL);
}


void Europe_commands::dispatch_cmd(std::vector<char*>& cmd_msg)
{
	System system;
	sscanf(cmd_msg[0], "%d", &system);

	switch (system)
	{
		case ACOUSTIC_PING:  //printf("Acoustic ping\n");
					         break;

		case IO_SYSTEM:  io_command(cmd_msg);
			             break;

		case NGC_SYSTEM: ngc_command(cmd_msg);
			             break;

		case TASK_SYSTEM: task_command(cmd_msg);
			              break;
	}

}


void Europe_commands::io_command(std::vector<char*>& cmd_msg)
{
	IO_cmd io_cmd;
	int index, int_value;
	double double_value;

	sscanf(cmd_msg[1], "%d", &io_cmd);


	IO_europe_status io_status;
	io_status = status->io_status.get();

	switch (io_cmd)
	{
		case SET_DIGITAL: sscanf(cmd_msg[2], "%d", &index);
						  sscanf(cmd_msg[3], "%d", &int_value);
						  io_status.digitalOutput[index] = int_value;
						  break;

		case SET_ANALOG: sscanf(cmd_msg[2], "%d", &index);
			             sscanf(cmd_msg[3], "%lf", &double_value);
						 io_status.analogOutput[index] = double_value;
						 break;

		case AUTO_START: io_status.autoStartStop = 1;
						 break;

		case AUTO_STOP:  io_status.autoStartStop = 2;
						 break;
	}
	
	status->io_status.set(io_status);
}


void Europe_commands::ngc_command(std::vector<char*>& cmd_msg)
{

}


void Europe_commands::task_command(std::vector<char*>& cmd_msg)
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



/*
 * Tasks.cpp
 *
 *  Created on: Feb 15, 2019
 *      Author: mx
 */

#include "Tasks.h"



Tasks::Tasks(const char *name,NetworkManager &nm, Europe_status* Status):RobotThread(name,SCHED_FIFO,TASKS_THREAD_PRIORITY,start_tasks)
{
	networkManager=&nm;

	status = Status;

	cmd=new CommLink( "Task_cmd" , OVERRIDE );
	cmd->open( networkManager->ROBOT_IP , networkManager->TASKS_ROBOT_HMI_PORT_IN ,
			   networkManager->HMI_IP   , networkManager->TASKS_HMI_PORT_OUT );
	cmd->create();


	int i=0;

	// Angular tasks
	
	angFilter = new AngFilter("AngFilter", status->filter_Ang_status, status->ngc_status, status->time_status);
	tasks.push_back(angFilter);
	taskMap[i][0] = INIT_ANG_FILTER;  taskMap[i++][1] = tasks.size() - 1;
	
	
	rawAngFromFOG = new RawAngFromFOG("rawAngFromFOG", status->raw_Ang_From_FOG_status, status->fog_status, status->ngc_status);
	rawAngFromFOG->setStatus(TASK_RUNNING);
	angSensorsToRaw.push_back(rawAngFromFOG);
	tasks.push_back(rawAngFromFOG);

	rawAngFromAHRS = new RawAngFromAHRS("RawAngFromAHRS", status->raw_Ang_From_AHRS_status, status->gps_ahrs_status, status->ngc_status);
	angSensorsToRaw.push_back(rawAngFromAHRS);
	tasks.push_back(rawAngFromAHRS);
	

	actualAngFromRaw = new ActualAngFromRaw("ActualAngFromRaw", status->actual_Ang_From_Raw_status, status->ngc_status);
	actualAngFromRaw->setStatus(TASK_RUNNING);
	tasks.push_back(actualAngFromRaw);

	actualAngFromFilter = new ActualAngFromFilter("ActualAngFromFilter", status->actual_Ang_From_Filter_status, status->ngc_status);
	tasks.push_back(actualAngFromFilter);
	
	

	
	


	// Horizontal velocity tasks
	/*
	horVelFilter = new HorVelFilter("HorVelFilter", status);
	tasks.push_back(horVelFilter);
	taskMap[i][0] = INIT_HOR_VEL_FILTER;  taskMap[i++][1] = tasks.size() - 1;

	actualHorVelFromRaw = new ActualHorVelFromRaw("ActualHorVelFromRaw", status);
	actualHorVelFromRaw->setStatus(TASK_RUNNING);
	tasks.push_back(actualHorVelFromRaw);

	actualHorVelFromFilter = new ActualHorVelFromFilter("ActualHorVelFromFilter", status);
	tasks.push_back(actualHorVelFromFilter);
	*/
	/*

	// Horizontal position tasks
	horPosFilter = new HorPosFilter("HorPosFilter", status);
	tasks.push_back(horPosFilter);
	taskMap[i][0] = INIT_HOR_POS_FILTER;  taskMap[i++][1] = tasks.size() - 1;

	actualHorPosFromRaw = new ActualHorPosFromRaw("ActualHorPosFromRaw", status->ngc_status, status->task_status);
	actualHorPosFromRaw->setStatus(TASK_RUNNING);
	tasks.push_back(actualHorPosFromRaw);

	actualHorPosFromFilter = new ActualHorPosFromFilter("ActualHorPosFromFilter", status);
	tasks.push_back(actualHorPosFromFilter);



	// Angular velocity control
	angVelControl = new AngVelControl("AngVelControl", status);
	tasks.push_back(angVelControl);
	taskMap[i][0] = ANG_VEL_CONTROL;  taskMap[i++][1] = tasks.size() - 1;

	// Angular position control
	angPosControl = new AngPosControl("AngPosControl", status);
	tasks.push_back(angPosControl);
	taskMap[i][0] = ANG_POS_CONTROL;  taskMap[i++][1] = tasks.size() - 1;

	// Horizontal velocity control
	horVelControl = new HorVelControl("HorVelControl", status);
	tasks.push_back(horVelControl);
	taskMap[i][0] = HOR_VEL_CONTROL;  taskMap[i++][1] = tasks.size() - 1;

	// Horizontal position control
	horPosControl = new HorPosControl("HorPosControl", status);
	tasks.push_back(horPosControl);
	taskMap[i][0] = HOR_POS_CONTROL;  taskMap[i++][1] = tasks.size() - 1;




	// PathFollowing FA
	pathFollowing_FA = new PathFollowing_FA("PathFollowing_FA", status);
	tasks.push_back(pathFollowing_FA);
	taskMap[i][0] = PATH_FOLLOWING_FA_CONTROL;  taskMap[i++][1] = tasks.size() - 1;

	pathFollowing_UA = new PathFollowing_UA("PathFollowing_UA", status);
	tasks.push_back(pathFollowing_UA);
	taskMap[i][0] = PATH_FOLLOWING_UA_CONTROL;  taskMap[i++][1] = tasks.size() - 1;


	*/

	/*


	


	

	rawHorVelFromDVL=new RawHorVelFromDVL("RawHorVelFromDVL",status);
	rawHorVelFromDVL->setStatus(TASK_RUNNING);
	horVelSensorsToRaw.push_back(rawHorVelFromDVL);
	tasks.push_back(rawHorVelFromDVL);

	rawHorVelFromGPS=new RawHorVelFromGPS("RawHorVelFromGPS",status);
	horVelSensorsToRaw.push_back(rawHorVelFromGPS);
	tasks.push_back(rawHorVelFromGPS);

	rawHorVelFromUSBL=new RawHorVelFromUSBL("RawHorVelFromUSBL",status);
	horVelSensorsToRaw.push_back(rawHorVelFromUSBL);
	tasks.push_back(rawHorVelFromUSBL);

	


	

	rawHorPosFromGPS=new RawHorPosFromGPS("RawHorPosFromGPS",status);
	rawHorPosFromGPS->setStatus(TASK_RUNNING);
	horPosSensorsToRaw.push_back(rawHorPosFromGPS);
	tasks.push_back(rawHorPosFromGPS);

	rawHorPosFromUSBL=new RawHorPosFromUSBL("RawHorPosFromUSBL",status);
	horPosSensorsToRaw.push_back(rawHorPosFromUSBL);
	tasks.push_back(rawHorPosFromUSBL);

	


	// Vertical velocity tasks
	verVelFilter=new VerVelFilter("VerVelFilter",status);
	tasks.push_back(verVelFilter);
	taskMap[i][0]=INIT_VER_VEL_FILTER;  taskMap[i++][1]=tasks.size()-1;

	rawVerVelFromDVL=new RawVerVelFromDVL("RawVerVelFromDVL",status);
	rawVerVelFromDVL->setStatus(TASK_RUNNING);
	verVelSensorsToRaw.push_back(rawVerVelFromDVL);
	tasks.push_back(rawVerVelFromDVL);

	rawVerVelFromCTD=new RawVerVelFromCTD("RawVerVelFromCTD",status);
	verVelSensorsToRaw.push_back(rawVerVelFromCTD);
	tasks.push_back(rawVerVelFromCTD);

	rawVerVelFromPA500=new RawVerVelFromPA500("RawVerVelFromPA500",status);
	verVelSensorsToRaw.push_back(rawVerVelFromPA500);
	tasks.push_back(rawVerVelFromPA500);

	rawVerVelFromECHO=new RawVerVelFromECHO("RawVerVelFromECHO",status);
	verVelSensorsToRaw.push_back(rawVerVelFromECHO);
	tasks.push_back(rawVerVelFromECHO);

	rawVerVelFromUSBL=new RawVerVelFromUSBL("RawVerVelFromUSBL",status);
	verVelSensorsToRaw.push_back(rawVerVelFromUSBL);
	tasks.push_back(rawVerVelFromUSBL);

	actualVerVelFromRaw=new ActualVerVelFromRaw("ActualVerVelFromRaw",status);
	actualVerVelFromRaw->setStatus(TASK_RUNNING);
	tasks.push_back(actualVerVelFromRaw);

	actualVerVelFromFilter=new ActualVerVelFromFilter("ActualVerVelFromFilter",status);
	tasks.push_back(actualVerVelFromFilter);


	// Vertical position tasks
	verPosFilter=new VerPosFilter("VerPosFilter",status);
	tasks.push_back(verPosFilter);
	taskMap[i][0]=INIT_VER_POS_FILTER;  taskMap[i++][1]=tasks.size()-1;

	rawVerPosFromCTD=new RawVerPosFromCTD("RawVerPosFromCTD",status);
	rawVerPosFromCTD->setStatus(TASK_RUNNING);
	verPosSensorsToRaw.push_back(rawVerPosFromCTD);
	tasks.push_back(rawVerPosFromCTD);

	rawVerPosFromUSBL=new RawVerPosFromUSBL("RawVerPosFromUSBL",status);
	verPosSensorsToRaw.push_back(rawVerPosFromUSBL);
	tasks.push_back(rawVerPosFromUSBL);

	actualVerPosFromRaw=new ActualVerPosFromRaw("ActualVerPosFromRaw",status);
	actualVerPosFromRaw->setStatus(TASK_RUNNING);
	tasks.push_back(actualVerPosFromRaw);

	actualVerPosFromFilter=new ActualVerPosFromFilter("ActualVerPosFromFilter",status);
	tasks.push_back(actualVerPosFromFilter);



	// Altitude tasks
	altFilter=new AltFilter("AltFilter",status);
	tasks.push_back(altFilter);
	taskMap[i][0]=INIT_ALT_FILTER;  taskMap[i++][1]=tasks.size()-1;

	rawAltFromPA500=new RawAltFromPA500("RawAltFromPA500",status);
	rawAltFromPA500->setStatus(TASK_RUNNING);
	altSensorsToRaw.push_back(rawAltFromPA500);
	tasks.push_back(rawAltFromPA500);

	rawAltFromECHO=new RawAltFromECHO("RawAltFromECHO",status);
	altSensorsToRaw.push_back(rawAltFromECHO);
	tasks.push_back(rawAltFromECHO);

	rawAltFromDVL=new RawAltFromDVL("RawAltFromDVL",status);
	altSensorsToRaw.push_back(rawAltFromDVL);
	tasks.push_back(rawAltFromDVL);

	actualAltFromRaw=new ActualAltFromRaw("ActualAltFromRaw",status);
	actualAltFromRaw->setStatus(TASK_RUNNING);
	tasks.push_back(actualAltFromRaw);

	actualAltFromFilter=new ActualAltFromFilter("ActualAltFromFilter",status);
	tasks.push_back(actualAltFromFilter);


	

	// Vertical velocity control
	verVelControl=new VerVelControl("VerVelControl",status);
	tasks.push_back(verVelControl);
	taskMap[i][0]=VER_VEL_CONTROL;  taskMap[i++][1]=tasks.size()-1;

	// Vertical position control
	verPosControl=new VerPosControl("VerPosControl",status);
	tasks.push_back(verPosControl);
	taskMap[i][0]=VER_POS_CONTROL;  taskMap[i++][1]=tasks.size()-1;

	// Altitude control
	altControl=new AltControl("AltControl",status);
	tasks.push_back(altControl);
	taskMap[i][0]=ALT_CONTROL;  taskMap[i++][1]=tasks.size()-1;



	

	*/

	printf("Tasks module started\n");
}


Tasks::~Tasks()
{
	for(uint32 i=0;i<tasks.size();i++)
		delete tasks[i];
}


void Tasks::execute()
{
	timespec tSleep;
	tSleep.tv_sec=CORE_SAMPLE_TIME_SEC;
	tSleep.tv_nsec=CORE_SAMPLE_TIME_NSEC;

	while(true)
	{
		read_cmd();

		for(uint32 i=0;i<tasks.size();i++)
		{
			tasks[i]->execute();
		}

		
		nanosleep(&tSleep,NULL);
	}
}



void Tasks::read_cmd()
{
	Task_cmd_packet msg;
	int ret;

	do{
		ret=cmd->recv_message((char*)&msg);
		if (ret>0)
		{
			//printf("msg.cmd_type: %lli       msg.value: %lli\n", msg.cmd_type, msg.value);
			switch(msg.cmd_type)
			{

				case SET_ANG_RAW_FILTER:		if (((TaskStatus)msg.value) == SENSOR_RAW)
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

/*
				case SET_HOR_VEL_RAW_FILTER:	if (msg.value == SENSOR_RAW)
												{
													actualHorVelFromFilter->setStatus(TASK_OFF);
													actualHorVelFromRaw->setStatus(TASK_RUNNING);
												}
												else if (msg.value == SENSOR_FILTER)
												{
													actualHorVelFromRaw->setStatus(TASK_OFF);
													actualHorVelFromFilter->setStatus(TASK_RUNNING);
												}
												break;

				case SET_HOR_POS_RAW_FILTER:	if (msg.value == SENSOR_RAW)
												{
													actualHorPosFromFilter->setStatus(TASK_OFF);
													actualHorPosFromRaw->setStatus(TASK_RUNNING);
												}
												else if (msg.value == SENSOR_FILTER)
												{
													actualHorPosFromRaw->setStatus(TASK_OFF);
													actualHorPosFromFilter->setStatus(TASK_RUNNING);
												}
												break;
*/

				case INIT_ANG_FILTER:			angFilter->setStatus(TASK_INIT);
												break;
/*
				case INIT_HOR_VEL_FILTER:		horVelFilter->setStatus(TASK_INIT);
												break;

				case INIT_HOR_POS_FILTER:		horPosFilter->setStatus(TASK_INIT);
												break;
*/
		/*		
				case ANG_VEL_CONTROL:			if (msg.value==CONTROLLER_OFF) angVelControl->setStatus(TASK_OFF);
												else if (msg.value==CONTROLLER_ON) angVelControl->setStatus(TASK_RUNNING);
												break;

				case ANG_POS_CONTROL:			if (msg.value==CONTROLLER_OFF) angPosControl->setStatus(TASK_OFF);
												else if (msg.value==CONTROLLER_ON) angPosControl->setStatus(TASK_RUNNING);
												break;

				case HOR_VEL_CONTROL:			if (msg.value==CONTROLLER_OFF) horVelControl->setStatus(TASK_OFF);
												else if (msg.value==CONTROLLER_ON) horVelControl->setStatus(TASK_RUNNING);
												break;

				case HOR_POS_CONTROL:			if (msg.value==CONTROLLER_OFF) horPosControl->setStatus(TASK_OFF);
												else if (msg.value==CONTROLLER_ON) horPosControl->setStatus(TASK_RUNNING);
												break;



				case PATH_FOLLOWING_FA_CONTROL: if (msg.value == CONTROLLER_OFF) pathFollowing_FA->setStatus(TASK_OFF);
												else if (msg.value == CONTROLLER_ON) pathFollowing_FA->setStatus(TASK_RUNNING);
												break;

				case PATH_FOLLOWING_UA_CONTROL: if (msg.value == CONTROLLER_OFF) pathFollowing_UA->setStatus(TASK_OFF);
												else if (msg.value == CONTROLLER_ON) pathFollowing_UA->setStatus(TASK_RUNNING);
												break;

*/
				/*
				case VER_VEL_CONTROL: if (msg.value==CONTROLLER_OFF) verVelControl->setStatus(TASK_OFF);
								      else if (msg.value==CONTROLLER_ON) verVelControl->setStatus(TASK_RUNNING);
								      break;

				case VER_POS_CONTROL: if (msg.value==CONTROLLER_OFF) verPosControl->setStatus(TASK_OFF);
								      else if (msg.value==CONTROLLER_ON) verPosControl->setStatus(TASK_RUNNING);
								      break;

				case ALT_CONTROL: if (msg.value==CONTROLLER_OFF) altControl->setStatus(TASK_OFF);
								      else if (msg.value==CONTROLLER_ON) altControl->setStatus(TASK_RUNNING);
								      break;


				*/



				case SET_ANG_SENSOR:     for(uint32 i=0;i<angSensorsToRaw.size();i++)
					                     	 angSensorsToRaw[i]->setStatus(TASK_OFF);
					
										 switch(msg.value)
									 	 {
									 	 	 case SENSOR_FOG:      rawAngFromFOG->setStatus(TASK_RUNNING);
									 	 	                       break;

											 case SENSOR_GPS_AHRS: rawAngFromAHRS->setStatus(TASK_RUNNING);
									 	 	                       break;
									 	 };

									 	 break;
/*
				case SET_HOR_VEL_SENSOR: for(uint32 i=0;i<horVelSensorsToRaw.size();i++)
										 	 horVelSensorsToRaw[i]->setStatus(TASK_OFF);
									 	 switch(msg.value)
									 	 {
									 	 	 case SENSOR_DVL: rawHorVelFromDVL->setStatus(TASK_RUNNING);
									 	 	 break;

									 	 	 case SENSOR_GPS_AHRS: rawHorVelFromGPS->setStatus(TASK_RUNNING);
									 	 	 break;

									 	 	 case SENSOR_USBL: rawHorVelFromUSBL->setStatus(TASK_RUNNING);
									 	 	 break;
									 	 };

									 	 break;

				case SET_HOR_POS_SENSOR: for(uint32 i=0;i<horPosSensorsToRaw.size();i++)
					                     	 horPosSensorsToRaw[i]->setStatus(TASK_OFF);
									 	 switch(msg.value)
									 	 {
									 	 	 case SENSOR_GPS_AHRS: rawHorPosFromGPS->setStatus(TASK_RUNNING);
									 	 	 break;

									 	 	 case SENSOR_USBL:     rawHorPosFromUSBL->setStatus(TASK_RUNNING);
									 	 	 break;
									 	 };

									 	 break;


				 case SET_VER_VEL_SENSOR: for(uint32 i=0;i<verVelSensorsToRaw.size();i++)
					 	 	 	 	 	  	  verVelSensorsToRaw[i]->setStatus(TASK_OFF);
									 	  switch(msg.value)
									 	  {
									 	  	  case SENSOR_DVL: rawVerVelFromDVL->setStatus(TASK_RUNNING);
									 	  	  break;

									 	  	  case SENSOR_CTD: rawVerVelFromCTD->setStatus(TASK_RUNNING);
									 	  	  break;

									 	  	  case SENSOR_PA500: rawVerVelFromPA500->setStatus(TASK_RUNNING);
									 	  	  break;

									 	  	  case SENSOR_ECHOLOGGER: rawVerVelFromECHO->setStatus(TASK_RUNNING);
									 	  	  break;

									 	  	  case SENSOR_USBL: rawVerVelFromUSBL->setStatus(TASK_RUNNING);
									 	  	  break;
									 	  };

									 	 break;


				 case SET_VER_POS_SENSOR: for(uint32 i=0;i<verPosSensorsToRaw.size();i++)
					 	 	 	 	 	  	  verPosSensorsToRaw[i]->setStatus(TASK_OFF);
									 	  switch(msg.value)
									 	  {
									 	  	  case SENSOR_CTD:  rawVerPosFromCTD->setStatus(TASK_RUNNING);
									 	  	  break;

									 	  	  case SENSOR_USBL: rawVerPosFromUSBL->setStatus(TASK_RUNNING);
									 	  	  break;
									 	  };

									 	  break;


				 case SET_ALT_SENSOR: for(uint32 i=0;i<altSensorsToRaw.size();i++)
					 	 	 	 	  	  altSensorsToRaw[i]->setStatus(TASK_OFF);
									  switch(msg.value)
									  {
									 	   	  case SENSOR_PA500:  rawAltFromPA500->setStatus(TASK_RUNNING);
									 	   	  break;

									 	   	  case SENSOR_ECHOLOGGER: rawAltFromECHO->setStatus(TASK_RUNNING);
									 	   	  break;

									 	   	  case SENSOR_DVL: rawAltFromDVL->setStatus(TASK_RUNNING);
									 	   	  break;
									  };

									 	  break;



				 

				


				 case SET_VER_VEL_RAW_FILTER: if (msg.value==SENSOR_RAW)
											  {
												 actualVerVelFromFilter->setStatus(TASK_OFF);
					 	 	 	 	 	 	 	 actualVerVelFromRaw->setStatus(TASK_RUNNING);
											  }
				                              else if (msg.value==SENSOR_FILTER)
				                              {
				                            	  actualVerVelFromRaw->setStatus(TASK_OFF);
				                            	  actualVerVelFromFilter->setStatus(TASK_RUNNING);
				                              }
											  break;


				 case SET_VER_POS_RAW_FILTER: if (msg.value==SENSOR_RAW)
											  {
												 actualVerPosFromFilter->setStatus(TASK_OFF);
					 	 	 	 	 	 	 	 actualVerPosFromRaw->setStatus(TASK_RUNNING);
											  }
				                              else if (msg.value==SENSOR_FILTER)
				                              {
				                            	  actualVerPosFromRaw->setStatus(TASK_OFF);
				                            	  actualVerPosFromFilter->setStatus(TASK_RUNNING);
				                              }
											  break;


				 case SET_ALT_RAW_FILTER: if (msg.value==SENSOR_RAW)
											  {
												 actualAltFromFilter->setStatus(TASK_OFF);
					 	 	 	 	 	 	 	 actualAltFromRaw->setStatus(TASK_RUNNING);
											  }
				                              else if (msg.value==SENSOR_FILTER)
				                              {
				                            	  actualAltFromRaw->setStatus(TASK_OFF);
				                            	  actualAltFromFilter->setStatus(TASK_RUNNING);
				                              }
											  break;


				

				

				case INIT_VER_VEL_FILTER: verVelFilter->setStatus(TASK_INIT);
				                          break;

				case INIT_VER_POS_FILTER: verPosFilter->setStatus(TASK_INIT);
				                          break;

				case INIT_ALT_FILTER:     altFilter->setStatus(TASK_INIT);
				                          break;
*/

				case SET_TASK_PARAM:      bool found=false;
										  int i=0;

										  while(i<num_tasks && !found)
											  if (msg.task_code==taskMap[i][0]) found=true;
											  else i++;

										  if (found) tasks[taskMap[i][1]]->set_param(msg.param, ((double)(msg.value))/Task_factor );

										  
								          break;



			};

		}

	}while(ret>0);
}



void* start_tasks(void *arg)
{
	Tasks* pThread=(Tasks*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}

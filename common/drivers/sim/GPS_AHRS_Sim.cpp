/*
 * GPS_AHRS_Sim.cpp
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */


#include "GPS_AHRS_Sim.h"


GPS_AHRS_Sim::GPS_AHRS_Sim(const char *name,NetworkManager &nm,SimStatus* s): DeviceSim(name,SCHED_FIFO,GPS_AHRS_THREAD_PRIORITY,start_gps_ahrs_sim,nm,s)
{
	tlm=new CommLink( "GPSAHRSSim_tlm" , UDP_PURE );

	tlm->open( networkManager->SIM_IP , networkManager->GPS_AHRS_SIM_PORT_OUT ,
			   networkManager->ROBOT_IP , networkManager->GPS_AHRS_ROBOT_PORT_IN);

	tlm->create();

	position_noise_avg=0.0;        position_noise_std=0.0;
	velocity_noise_avg=0.0;        velocity_noise_std=0.0;
	orientation_noise_avg=0.0;     orientation_noise_std=0.0;
	orientationRate_noise_avg=0.0; orientationRate_noise_std=0.0;
	acceleration_noise_avg=0.0;    acceleration_noise_std=0.0;

	position_noise=NULL;
	velocity_noise=NULL;
	orientation_noise=NULL;
	orientationRate_noise=NULL;
	acceleration_noise=NULL;

	position_noise_uw=NULL;
	velocity_noise_uw=NULL;

	lat0=0.0;  lon0=0.0;
	x0=0.0;    y0=0.0;
	utmzone=0.0;
	utmzone_char='\0';
}


GPS_AHRS_Sim::~GPS_AHRS_Sim()
{
	tlm->terminate();
	delete tlm;
}




int GPS_AHRS_Sim::init()
{
	FILE *f;
	char name_dir_file[256];
	char value[100],value2[100];
	double val;

	sprintf(name_dir_file,"%s%s",CONFIGURATION_SIM_FILES_DIRECTORY,CONFIGURATION_GPS_AHRS_FILE);

	if((f=fopen(name_dir_file,"r"))!=NULL)
	{
		while(!feof(f))
		{
			fscanf(f,"%s %s %lf\n",value,value2,&val);

			if (strcmp(value,"position_noise_avg")==0) position_noise_avg=val;
			if (strcmp(value,"position_noise_std")==0) position_noise_std=val;

			if (strcmp(value,"velocity_noise_avg")==0) velocity_noise_avg=val;
			if (strcmp(value,"velocity_noise_std")==0) velocity_noise_std=val;

			if (strcmp(value,"orientation_noise_avg")==0) orientation_noise_avg=val * M_PI/180.0;
			if (strcmp(value,"orientation_noise_std")==0) orientation_noise_std=val * M_PI / 180.0;

			if (strcmp(value,"orientationRate_noise_avg")==0) orientationRate_noise_avg=val * M_PI / 180.0;
			if (strcmp(value,"orientationRate_noise_std")==0) orientationRate_noise_std=val * M_PI / 180.0;

			if (strcmp(value,"acceleration_noise_avg")==0) acceleration_noise_avg=val;
			if (strcmp(value,"acceleration_noise_std")==0) acceleration_noise_std=val;
		}

		fclose(f);

		position_noise=new std::normal_distribution<double>( position_noise_avg , position_noise_std );
		velocity_noise=new std::normal_distribution<double>( velocity_noise_avg , velocity_noise_std );
		orientation_noise=new std::normal_distribution<double>( orientation_noise_avg , orientation_noise_std );
		orientationRate_noise=new std::normal_distribution<double>( orientationRate_noise_avg , orientationRate_noise_std );
		acceleration_noise=new std::normal_distribution<double>( acceleration_noise_avg , acceleration_noise_std );

		position_noise_uw=new std::normal_distribution<double>( 0.0 , 20.0 );
		velocity_noise_uw=new std::normal_distribution<double>( 0.0 , 10.0 );
	}

	else
	{
		printf("GPS_AHRS_Sim::init - File error: %s\n",name_dir_file);
		return -1;
	}



	sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,CONFIGURATION_GPS_AHRS_FILE);

	if((f=fopen(name_dir_file,"r"))!=NULL)
	{
		fscanf(f,"%s\n",value);

		while(!feof(f))
		{
			fscanf(f,"%s %s %lf\n",value,value2,&val);

			if (strcmp(value,"origin_latitude")==0) lat0=val;
			if (strcmp(value,"origin_longitude")==0) lon0=val;
		}

		fclose(f);

		compute_xy_from_lat_lon(lat0,lon0,x0,y0,utmzone,utmzone_char);

		return 0;
	}

	else
	{
		printf("GPS_AHRS_Sim::init - File error: %s\n",name_dir_file);
		return -1;
	}

	return 0;
}

void GPS_AHRS_Sim::execute()
{
	timespec tSleep;
	tSleep.tv_sec=GPS_AHRS_SLEEP_SEC;
	tSleep.tv_nsec=GPS_AHRS_SLEEP_NSEC;

	while(running)
	{
		send_tlm();

		utcTime++;

		nanosleep(&tSleep,NULL);
	}
}



void GPS_AHRS_Sim::read_cmd()
{

}


void GPS_AHRS_Sim::send_tlm()
{
	//GPS_AHRS_tlm_packet msg;

	double noise;
	double phi_measure,theta_measure,psi_measure;
	double p_measure,q_measure,r_measure;
	double x_measure,y_measure,xDot_measure,yDot_measure,speed,course;
	double xAccel_measure,yAccel_measure,zAccel_measure;
	double lat,lon;
	int pos_valid,vel_valid;


	noise=(*orientation_noise)(random_gen);   phi_measure=simStatus->phi + noise;  
	noise=(*orientation_noise)(random_gen);   theta_measure=simStatus->theta + noise;
	noise=(*orientation_noise)(random_gen);   psi_measure=simStatus->psi + noise;


	noise=(*orientationRate_noise)(random_gen);   p_measure=simStatus->p + noise;
	noise=(*orientationRate_noise)(random_gen);   q_measure=simStatus->q + noise;
	noise=(*orientationRate_noise)(random_gen);   r_measure=simStatus->r + noise;

	if (simStatus->z <= uw_trh)
	{
		noise=(*position_noise)(random_gen);   x_measure=simStatus->x + noise;
		noise=(*position_noise)(random_gen);   y_measure=simStatus->y + noise;
	}
	else
	{
		noise=(*position_noise_uw)(random_gen);   x_measure=simStatus->x + noise;
		noise=(*position_noise_uw)(random_gen);   y_measure=simStatus->y + noise;
	}

	compute_lat_lon_from_xy(x0+x_measure,y0+y_measure,utmzone,utmzone_char,lat,lon);

	if (simStatus->z <= uw_trh)
	{
		noise=(*velocity_noise)(random_gen);   xDot_measure=simStatus->xDot + noise;
		noise=(*velocity_noise)(random_gen);   yDot_measure=simStatus->yDot + noise;
	}
	else
	{
		noise=(*velocity_noise_uw)(random_gen);   xDot_measure=simStatus->xDot + noise;
		noise=(*velocity_noise_uw)(random_gen);   yDot_measure=simStatus->yDot + noise;
	}

	speed=sqrt(xDot_measure*xDot_measure + yDot_measure*yDot_measure);
	course=atan2(yDot_measure,xDot_measure);

	noise=(*acceleration_noise)(random_gen); xAccel_measure=simStatus->xAcc + noise;
	noise=(*acceleration_noise)(random_gen); yAccel_measure=simStatus->yAcc + noise;
	noise=(*acceleration_noise)(random_gen); zAccel_measure=simStatus->zAcc + noise;

	pos_valid=1;
	vel_valid=1;
	if (simStatus->z > uw_trh)
	{
		pos_valid=0;
		vel_valid=0;
	}

	
	gps_ahrs_status.roll.value = phi_measure;
	gps_ahrs_status.pitch.value = theta_measure;
	gps_ahrs_status.heading.value = psi_measure;
	gps_ahrs_status.xGyro.value = p_measure;
	gps_ahrs_status.yGyro.value = q_measure;
	gps_ahrs_status.zGyro.value = r_measure;
	gps_ahrs_status.latitude.value = lat;
	gps_ahrs_status.longitude.value = lon;
	gps_ahrs_status.heightAboveEllipsoid.value = 0.0;
	gps_ahrs_status.heightAboveMSL.value = 0.0;
	gps_ahrs_status.northVel.value = xDot_measure;
	gps_ahrs_status.eastVel.value = yDot_measure;
	gps_ahrs_status.downVel.value = 0.0;
	gps_ahrs_status.speed.value = speed;
	gps_ahrs_status.groundSpeed.value = speed;
	gps_ahrs_status.course.value = course;
	gps_ahrs_status.xAccel.value = xAccel_measure;
	gps_ahrs_status.yAccel.value = yAccel_measure;
	gps_ahrs_status.zAccel.value = zAccel_measure;
	gps_ahrs_status.llhPositionValidFlags = pos_valid;
	gps_ahrs_status.nedVelocityValidFlags = vel_valid;
	
	
	//printf("lat: %lf   lon: %lf   valid: %d\n",lat,lon,pos_valid);


	GPS_AHRS_tlm_packet msg;
	gps_ahrs_status.compose_tlm_packet(msg);
	tlm->send_message((char*)&msg, sizeof(msg));


	/*
	char msg[1024];

	sprintf(msg, "101 %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %lld %d\r\n",
		psi_measure * (180.0 / M_PI), phi_measure * (180.0 / M_PI), theta_measure * (180.0 / M_PI),
		lat, lon,
		0.0, 0.0, 0.0, 0.0, pos_valid,
		xAccel_measure, yAccel_measure, zAccel_measure,
		p_measure, q_measure, r_measure,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		xDot_measure, yDot_measure, 0.0, speed, speed, course, 0.0, 0.0, vel_valid,
		utcTime, 1);
		*/
	//printf("%s",msg);

	//tlm->send_message(msg,sizeof(msg));
}




void* start_gps_ahrs_sim(void *arg)
{
	GPS_AHRS_Sim* pThread=(GPS_AHRS_Sim*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}



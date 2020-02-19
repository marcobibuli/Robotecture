/*
 * GPS_AHRS.cpp
 *
 *  Created on: Jul 24, 2018
 *      Author: mx
 */

#include "GPS_AHRS.h"

GPS_AHRS::GPS_AHRS(const char *name,NetworkManager &nm, DataAccess<GPS_AHRS_status>& Gps_ahrs_access, DataAccess<Time_status>& Time_access):Device(name,SCHED_FIFO,GPS_AHRS_THREAD_PRIORITY,start_gps_ahrs,nm)
{
	strcpy(configFileName,CONFIGURATION_GPS_AHRS_FILE);

	time_access = &Time_access;
	gps_ahrs_access = &Gps_ahrs_access;
	/*
	cmd = new CommLink("GPS_AHRS_cmd", UDP_PURE);
	cmd->open(networkManager->ROBOT_IP, networkManager->GPS_AHRS_ROBOT_PORT_OUT,
		      networkManager->GPS_AHRS_IP, networkManager->GPS_AHRS_PORT_IN);


	tlm=new CommLink( "GPS_AHRS_tlm" , UDP_PURE );
	tlm->open( networkManager->ROBOT_IP , networkManager->GPS_AHRS_ROBOT_PORT_IN,
			   networkManager->GPS_AHRS_IP, networkManager->GPS_AHRS_PORT_OUT );
	//tlm->create();
	*/
	

	

	/*
	tlmHMI = new CommLink("GPS_AHRS_tlmHMI", OVERRIDE);
	tlmHMI->open(networkManager->ROBOT_IP, networkManager->GPS_AHRS_ROBOT_HMI_PORT_OUT,
				 networkManager->HMI_IP, networkManager->GPS_AHRS_HMI_PORT_IN);
	tlmHMI->create();
	*/

	lat0=0.0;  lon0=0.0;
	x0=0.0;    y0=0.0;
	utmzone=0.0;
	utmzone_char='\0';

	ahrs_status = DEVICE_RUNNING;
	gps_status = DEVICE_RUNNING;
} 

GPS_AHRS::~GPS_AHRS()
{

}


int GPS_AHRS::init_sim()
{
	printf("GPS_AHRS::init_sim()\n");
	running_sim=true;

	device_status=DEVICE_RUNNING;

	int ret=init_pos();


	tlm = new CommLink("GPS_AHRS_tlmSim", UDP_PURE);
	tlm->open(networkManager->ROBOT_IP, networkManager->GPS_AHRS_ROBOT_SIM_PORT_IN,
		      networkManager->SIM_IP, networkManager->GPS_AHRS_SIM_PORT_OUT);
	tlm->create();


	return ret;
}


int GPS_AHRS::init_act()
{
	printf("GPS_AHRS::init_act()\n");
	running_act=true;


	device_status=DEVICE_RUNNING;

	char s_ip[256];
	int a, b, c, ip;
	strcpy(s_ip, networkManager->ROBOT_IP);
	sscanf(s_ip, "%d.%d.%d.%d", &a, &b, &c, &ip);

	char msg[256];
	sprintf(msg, "10001 %d %d",ip,networkManager->GPS_AHRS_ROBOT_SIM_PORT_IN);
	cmd->send_message(msg, strlen(msg));

	sleep(1);

	// aggiunta per echologger
	char msg2[256];
	sprintf(msg2, "10001 210 2969");
	cmd->send_message(msg2, strlen(msg2));
	

	int ret=init_pos();

	return ret;
}

int GPS_AHRS::init_pos()
{
	FILE *f;
	char value[100],value2[100],name_dir_file[256];
	double val;

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
		printf("GPS_AHRS::init - File error: %s\n",name_dir_file);
		return -1;
	}

	return 0;
}


void GPS_AHRS::execute_sim()
{
	timespec tSleep;
	tSleep.tv_sec=GPS_AHRS_SLEEP_SEC;
	tSleep.tv_nsec=GPS_AHRS_SLEEP_NSEC;

	while(running_sim)
	{
		read_sim_tlm();

		//raw_data();

		//send_telemetry();

		nanosleep(&tSleep,NULL);
	}
}


void GPS_AHRS::read_sim_tlm()
{

	GPS_AHRS_tlm_packet tp;
	GPS_AHRS_status gps_ahrs_status;
	Time_status time_status;

	int ret;

	do{
		ret=tlm->recv_message((char*)&tp);
		if (ret>0)
		{
			/*
			NGC_status ngc_status;
			ngc_status = status->ngc_status.get();

			ngc_status.pose.actual.x.value = msg.x;
			ngc_status.pose.actual.y.value = msg.y;
			ngc_status.pose.actual.psi.value = msg.yaw;
			ngc_status.velocity_body.actual.u.value = msg.u;
			ngc_status.velocity_body.actual.v.value = msg.v;
			ngc_status.velocity_body.actual.r.value = msg.r;

			status->ngc_status.set(ngc_status);

			printf("pos: %lf  %lf  %lf         vel: %lf  %lf  %lf\n", msg.x , msg.y , msg.yaw , msg.u , msg.v , msg.r );
			*/


			gps_ahrs_status = gps_ahrs_access->get();
			time_status = time_access->get();

			double roll = (((double)(tp.roll)) / AHRS_factor) * M_PI / 180.0;
			double pitch = (((double)(tp.pitch)) / AHRS_factor) * M_PI / 180.0;
			double heading = (((double)(tp.heading)) / AHRS_factor) * M_PI / 180.0;

			double xGyro = (((double)(tp.xGyro)) / AHRS_factor) * M_PI / 180.0;
			double yGyro = (((double)(tp.yGyro)) / AHRS_factor) * M_PI / 180.0;
			double zGyro = (((double)(tp.zGyro)) / AHRS_factor) * M_PI / 180.0;

			double latitude = (((double)(tp.latitude)) / GPS_factor);
			double longitude = (((double)(tp.longitude)) / GPS_factor);

			double heightAboveEllipsoid = (((double)(tp.heightAboveEllipsoid)) / AHRS_factor);
			double heightAboveMSL = (((double)(tp.heightAboveMSL)) / AHRS_factor);

			double northVel = (((double)(tp.northVel)) / AHRS_factor);
			double eastVel = (((double)(tp.eastVel)) / AHRS_factor);
			double downVel = (((double)(tp.downVel)) / AHRS_factor);
			double speed = (((double)(tp.speed)) / AHRS_factor);
			double groundSpeed = (((double)(tp.groundSpeed)) / AHRS_factor);
			double course = (((double)(tp.course)) / AHRS_factor);

			double xAccel = (((double)(tp.xAccel)) / AHRS_factor);
			double yAccel = (((double)(tp.yAccel)) / AHRS_factor);
			double zAccel = (((double)(tp.zAccel)) / AHRS_factor);

			double horizontalAccuracy = (((double)(tp.horizontalAccuracy)) / AHRS_factor);
			double verticalAccuracy = (((double)(tp.verticalAccuracy)) / AHRS_factor);
			double speedAccuracy = (((double)(tp.speedAccuracy)) / AHRS_factor);
			double courseAccuracy = (((double)(tp.courseAccuracy)) / AHRS_factor);

			int64 llhPositionValidFlags = tp.llhPositionValidFlags;
			int64 nedVelocityValidFlags = tp.nedVelocityValidFlags;
			


			gps_ahrs_status.llhPositionValidFlags = llhPositionValidFlags;  int64 pos_valid = (llhPositionValidFlags != 0);
			gps_ahrs_status.nedVelocityValidFlags = nedVelocityValidFlags;  int64 vel_valid = (nedVelocityValidFlags != 0);


			gps_ahrs_status.roll.value = modpi(roll * M_PI / 180.0);				gps_ahrs_status.roll.timeStamp = time_status.timeStamp;										gps_ahrs_status.roll.valid = true;
			gps_ahrs_status.pitch.value = modpi(pitch * M_PI / 180.0);				gps_ahrs_status.pitch.timeStamp = time_status.timeStamp;									gps_ahrs_status.pitch.valid = true;
			gps_ahrs_status.heading.value = modpi(heading * M_PI / 180.0);			gps_ahrs_status.heading.timeStamp = time_status.timeStamp;									gps_ahrs_status.heading.valid = true;
			gps_ahrs_status.latitude.value = latitude;								if (pos_valid) gps_ahrs_status.latitude.timeStamp = time_status.timeStamp;					gps_ahrs_status.latitude.valid = pos_valid;
			gps_ahrs_status.longitude.value = longitude;								if (pos_valid) gps_ahrs_status.longitude.timeStamp = time_status.timeStamp;					gps_ahrs_status.longitude.valid = pos_valid;
			gps_ahrs_status.heightAboveEllipsoid.value = heightAboveEllipsoid;		if (pos_valid) gps_ahrs_status.heightAboveEllipsoid.timeStamp = time_status.timeStamp;		gps_ahrs_status.heightAboveEllipsoid.valid = pos_valid;
			gps_ahrs_status.heightAboveMSL.value = heightAboveMSL;					if (pos_valid) gps_ahrs_status.heightAboveMSL.timeStamp = time_status.timeStamp;			gps_ahrs_status.heightAboveMSL.valid = pos_valid;
			gps_ahrs_status.horizontalAccuracy.value = heightAboveMSL;				if (pos_valid) gps_ahrs_status.horizontalAccuracy.timeStamp = time_status.timeStamp;		gps_ahrs_status.horizontalAccuracy.valid = pos_valid;
			gps_ahrs_status.verticalAccuracy.value = heightAboveMSL;				if (pos_valid) gps_ahrs_status.verticalAccuracy.timeStamp = time_status.timeStamp;			gps_ahrs_status.verticalAccuracy.valid = pos_valid;
			gps_ahrs_status.xGyro.value = xGyro;									gps_ahrs_status.xGyro.timeStamp = time_status.timeStamp;									gps_ahrs_status.xGyro.valid = true;
			gps_ahrs_status.yGyro.value = yGyro;									gps_ahrs_status.yGyro.timeStamp = time_status.timeStamp;									gps_ahrs_status.yGyro.valid = true;
			gps_ahrs_status.zGyro.value = zGyro;									gps_ahrs_status.zGyro.timeStamp = time_status.timeStamp;									gps_ahrs_status.zGyro.valid = true;
			gps_ahrs_status.northVel.value = northVel;								if (vel_valid) gps_ahrs_status.northVel.timeStamp = time_status.timeStamp;					gps_ahrs_status.northVel.valid = vel_valid;
			gps_ahrs_status.eastVel.value = eastVel;								if (vel_valid) gps_ahrs_status.eastVel.timeStamp = time_status.timeStamp;					gps_ahrs_status.eastVel.valid = vel_valid;
			gps_ahrs_status.downVel.value = downVel;								if (vel_valid) gps_ahrs_status.downVel.timeStamp = time_status.timeStamp;					gps_ahrs_status.downVel.valid = vel_valid;
			gps_ahrs_status.speed.value = speed;									if (vel_valid) gps_ahrs_status.speed.timeStamp = time_status.timeStamp;						gps_ahrs_status.speed.valid = vel_valid;
			gps_ahrs_status.groundSpeed.value = groundSpeed;						if (vel_valid) gps_ahrs_status.groundSpeed.timeStamp = time_status.timeStamp;				gps_ahrs_status.groundSpeed.valid = vel_valid;
			gps_ahrs_status.course.value = modpi(course * M_PI / 180.0);			if (vel_valid) gps_ahrs_status.course.timeStamp = time_status.timeStamp;					gps_ahrs_status.course.valid = vel_valid;
			gps_ahrs_status.speedAccuracy.value = speedAccuracy;					if (vel_valid) gps_ahrs_status.speedAccuracy.timeStamp = time_status.timeStamp;				gps_ahrs_status.speedAccuracy.valid = vel_valid;
			gps_ahrs_status.courseAccuracy.value = courseAccuracy;					if (vel_valid) gps_ahrs_status.courseAccuracy.timeStamp = time_status.timeStamp;			gps_ahrs_status.courseAccuracy.valid = vel_valid;



			gps_ahrs_status.ahrs_status = ahrs_status;
			gps_ahrs_status.gps_status = gps_status;

			gps_ahrs_status.lat0.value = lat0;		gps_ahrs_status.lat0.timeStamp = time_status.timeStamp;		gps_ahrs_status.lat0.valid = true;
			gps_ahrs_status.lon0.value = lon0;		gps_ahrs_status.lon0.timeStamp = time_status.timeStamp;		gps_ahrs_status.lon0.valid = true;
			gps_ahrs_status.x0.value = x0;			gps_ahrs_status.x0.timeStamp = time_status.timeStamp;		gps_ahrs_status.x0.valid = true;
			gps_ahrs_status.y0.value = y0;			gps_ahrs_status.y0.timeStamp = time_status.timeStamp;		gps_ahrs_status.y0.valid = true;

			double x = 0.0, y = 0.0;
			compute_xy_from_lat_lon(gps_ahrs_status.latitude.value, gps_ahrs_status.longitude.value, x, y, utmzone, utmzone_char);

			gps_ahrs_status.x.value = x - x0;		if (pos_valid) gps_ahrs_status.x.timeStamp = time_status.timeStamp;		gps_ahrs_status.x.valid = pos_valid;
			gps_ahrs_status.y.value = y - y0;		if (pos_valid) gps_ahrs_status.y.timeStamp = time_status.timeStamp;		gps_ahrs_status.y.valid = pos_valid;


			double xDot = 0.0, yDot = 0.0, psi = 0.0;

			xDot = gps_ahrs_status.northVel.value;
			yDot = gps_ahrs_status.eastVel.value;
			psi = gps_ahrs_status.heading.value;

			double u = cos(psi) * xDot + sin(psi) * yDot;
			double v = -sin(psi) * xDot + cos(psi) * yDot;

			gps_ahrs_status.u.value = u;		if (vel_valid) gps_ahrs_status.u.timeStamp = time_status.timeStamp;		gps_ahrs_status.u.valid = vel_valid;
			gps_ahrs_status.v.value = v;		if (vel_valid) gps_ahrs_status.v.timeStamp = time_status.timeStamp;		gps_ahrs_status.v.valid = vel_valid;


			//printf("%lf  %lf\n", latitude, longitude);

			//time_status.utcTime = utcTime;

			gps_ahrs_access->set(gps_ahrs_status);
			time_access->set(time_status);

		}

		update_device_status(ret);


	}while(ret>0);

}


void GPS_AHRS::read_tlm()
{
	int ip;
	
	int llhValidFlag, nedValidFlag, utcFlag;

	double yaw, pitch, roll, lat, lon, heightAboveEllipsoid, heightAboveMSL, horizontalAccuracy, verticalAccuracy,
		xAcc, yAcc, zAcc, xGyro, yGyro, zGyro, xMag, yMag, zMag, rotMatrix[3][3], northVel, eastVel, downVel, speed, groundSpeed,
		course, speedAccuracy, headingAccuracy;

	int64 utcTime;

	bool pos_valid, vel_valid;

	GPS_AHRS_status gps_ahrs_status;
	Time_status time_status;


	char msg[1024];
	int ret;
	do {
		ret = tlm->recv_message(msg, sizeof(msg));
		if (ret > 0)
		{
			sscanf(msg, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %lld %d\r\n",
				   &ip,  &yaw, &roll, &pitch, &lat, &lon, &heightAboveEllipsoid, &heightAboveMSL, &horizontalAccuracy, &verticalAccuracy, &llhValidFlag,
				   &xAcc, &yAcc, &zAcc, &xGyro, &yGyro, &zGyro, &xMag, &yMag, &zMag, &rotMatrix[0][0], &rotMatrix[0][1], &rotMatrix[0][2], &rotMatrix[1][0], &rotMatrix[1][1], &rotMatrix[1][2],
				   &rotMatrix[2][0], &rotMatrix[2][1], &rotMatrix[2][2],	&northVel, &eastVel, &downVel, &speed, &groundSpeed, &course, &speedAccuracy, &headingAccuracy, &nedValidFlag, 
				   &utcTime, &utcFlag  );

			

			time_status = time_access->get();
			gps_ahrs_status =gps_ahrs_access->get();


			gps_ahrs_status.llhPositionValidFlags = llhValidFlag;  pos_valid = (llhValidFlag != 0);
			gps_ahrs_status.nedVelocityValidFlags = nedValidFlag;  vel_valid = (nedValidFlag != 0);
			

			gps_ahrs_status.roll.value = modpi(roll * M_PI / 180.0);				gps_ahrs_status.roll.timeStamp = time_status.timeStamp;										gps_ahrs_status.roll.valid = true;
			gps_ahrs_status.pitch.value = modpi(pitch * M_PI / 180.0);				gps_ahrs_status.pitch.timeStamp = time_status.timeStamp;									gps_ahrs_status.pitch.valid = true;
			gps_ahrs_status.heading.value = modpi(yaw * M_PI/180.0);				gps_ahrs_status.heading.timeStamp = time_status.timeStamp;									gps_ahrs_status.heading.valid = true;
			gps_ahrs_status.latitude.value = lat;									if (pos_valid) gps_ahrs_status.latitude.timeStamp = time_status.timeStamp;					gps_ahrs_status.latitude.valid = pos_valid;
			gps_ahrs_status.longitude.value = lon;									if (pos_valid) gps_ahrs_status.longitude.timeStamp = time_status.timeStamp;					gps_ahrs_status.longitude.valid = pos_valid;
			gps_ahrs_status.heightAboveEllipsoid.value = heightAboveEllipsoid;		if (pos_valid) gps_ahrs_status.heightAboveEllipsoid.timeStamp = time_status.timeStamp;		gps_ahrs_status.heightAboveEllipsoid.valid = pos_valid;
			gps_ahrs_status.heightAboveMSL.value = heightAboveMSL;					if (pos_valid) gps_ahrs_status.heightAboveMSL.timeStamp = time_status.timeStamp;			gps_ahrs_status.heightAboveMSL.valid = pos_valid;
			gps_ahrs_status.horizontalAccuracy.value = heightAboveMSL;				if (pos_valid) gps_ahrs_status.horizontalAccuracy.timeStamp = time_status.timeStamp;		gps_ahrs_status.horizontalAccuracy.valid = pos_valid;
			gps_ahrs_status.verticalAccuracy.value = heightAboveMSL;				if (pos_valid) gps_ahrs_status.verticalAccuracy.timeStamp = time_status.timeStamp;			gps_ahrs_status.verticalAccuracy.valid = pos_valid;
			gps_ahrs_status.xGyro.value = xGyro;									gps_ahrs_status.xGyro.timeStamp = time_status.timeStamp;									gps_ahrs_status.xGyro.valid = true;
			gps_ahrs_status.yGyro.value = yGyro;									gps_ahrs_status.yGyro.timeStamp = time_status.timeStamp;									gps_ahrs_status.yGyro.valid = true;
			gps_ahrs_status.zGyro.value = zGyro;									gps_ahrs_status.zGyro.timeStamp = time_status.timeStamp;									gps_ahrs_status.zGyro.valid = true;
			gps_ahrs_status.northVel.value = northVel;								if (vel_valid) gps_ahrs_status.northVel.timeStamp = time_status.timeStamp;					gps_ahrs_status.northVel.valid = vel_valid;
			gps_ahrs_status.eastVel.value = eastVel;								if (vel_valid) gps_ahrs_status.eastVel.timeStamp = time_status.timeStamp;					gps_ahrs_status.eastVel.valid = vel_valid;
			gps_ahrs_status.downVel.value = downVel;								if (vel_valid) gps_ahrs_status.downVel.timeStamp = time_status.timeStamp;					gps_ahrs_status.downVel.valid = vel_valid;
			gps_ahrs_status.speed.value = speed;									if (vel_valid) gps_ahrs_status.speed.timeStamp = time_status.timeStamp;						gps_ahrs_status.speed.valid = vel_valid;
			gps_ahrs_status.groundSpeed.value = groundSpeed;						if (vel_valid) gps_ahrs_status.groundSpeed.timeStamp = time_status.timeStamp;				gps_ahrs_status.groundSpeed.valid = vel_valid;
			gps_ahrs_status.course.value = modpi(course * M_PI / 180.0);			if (vel_valid) gps_ahrs_status.course.timeStamp = time_status.timeStamp;					gps_ahrs_status.course.valid = vel_valid;
			gps_ahrs_status.speedAccuracy.value = speedAccuracy;					if (vel_valid) gps_ahrs_status.speedAccuracy.timeStamp = time_status.timeStamp;				gps_ahrs_status.speedAccuracy.valid = vel_valid;
			gps_ahrs_status.courseAccuracy.value = headingAccuracy;					if (vel_valid) gps_ahrs_status.courseAccuracy.timeStamp = time_status.timeStamp;			gps_ahrs_status.courseAccuracy.valid = vel_valid;

			

			gps_ahrs_status.ahrs_status = ahrs_status;
			gps_ahrs_status.gps_status = gps_status;

			gps_ahrs_status.lat0.value = lat0;		gps_ahrs_status.lat0.timeStamp = time_status.timeStamp;		gps_ahrs_status.lat0.valid = true;
			gps_ahrs_status.lon0.value = lon0;		gps_ahrs_status.lon0.timeStamp = time_status.timeStamp;		gps_ahrs_status.lon0.valid = true;
			gps_ahrs_status.x0.value = x0;			gps_ahrs_status.x0.timeStamp = time_status.timeStamp;		gps_ahrs_status.x0.valid = true;
			gps_ahrs_status.y0.value = y0;			gps_ahrs_status.y0.timeStamp = time_status.timeStamp;		gps_ahrs_status.y0.valid = true;

			double x=0.0, y=0.0;
			compute_xy_from_lat_lon(gps_ahrs_status.latitude.value, gps_ahrs_status.longitude.value, x, y, utmzone, utmzone_char);

			gps_ahrs_status.x.value = x - x0;		if (pos_valid) gps_ahrs_status.x.timeStamp = time_status.timeStamp;		gps_ahrs_status.x.valid = pos_valid;
			gps_ahrs_status.y.value = y - y0;		if (pos_valid) gps_ahrs_status.y.timeStamp = time_status.timeStamp;		gps_ahrs_status.y.valid = pos_valid;


			double xDot = 0.0, yDot = 0.0 ,psi=0.0;

			xDot = gps_ahrs_status.northVel.value;
			yDot = gps_ahrs_status.eastVel.value;
			psi = gps_ahrs_status.heading.value;
			
			double u = cos(psi) * xDot + sin(psi) * yDot;
			double v = -sin(psi) * xDot + cos(psi) * yDot;

			gps_ahrs_status.u.value = u;		if (vel_valid) gps_ahrs_status.u.timeStamp = time_status.timeStamp;		gps_ahrs_status.u.valid = vel_valid;
			gps_ahrs_status.v.value = v;		if (vel_valid) gps_ahrs_status.v.timeStamp = time_status.timeStamp;		gps_ahrs_status.v.valid = vel_valid;

			
			printf("%lf  %lf\n",lat,lon);

			//time_status.utcTime = utcTime;

			gps_ahrs_access->set(gps_ahrs_status);
			time_access->set(time_status);
		}

		update_device_status(ret);

	} while (ret > 0);

}


void GPS_AHRS::execute_act()
{
	timespec tSleep;
	tSleep.tv_sec=GPS_AHRS_SLEEP_SEC;
	tSleep.tv_nsec=GPS_AHRS_SLEEP_NSEC;

	while(running_act)
	{
		read_tlm();

		raw_data();

		send_telemetry();

		nanosleep(&tSleep,NULL);
	}
}


void GPS_AHRS::raw_data()
{
	/*
	GPS_AHRS_status gps_ahrs_status;
	gps_ahrs_status = gps_ahrs_access->get();

	NGC_status ngc_status;
	ngc_status = status->ngc_status.get();

	

	ngc_status.velocity_body.raw.p = gps_ahrs_status.xGyro;
	ngc_status.velocity_body.raw.q = gps_ahrs_status.yGyro;
	ngc_status.velocity_body.raw.r = gps_ahrs_status.zGyro;

	ngc_status.velocity_inertial.raw.xDot = gps_ahrs_status.northVel;
	ngc_status.velocity_inertial.raw.yDot = gps_ahrs_status.eastVel;

	ngc_status.pose.raw.phi = gps_ahrs_status.roll;
	ngc_status.pose.raw.theta = gps_ahrs_status.pitch;
	ngc_status.pose.raw.psi = gps_ahrs_status.heading;
	ngc_status.pose.raw.x = gps_ahrs_status.x;
	ngc_status.pose.raw.y = gps_ahrs_status.y;





	ngc_status.velocity_body.raw.u = gps_ahrs_status.u;
	ngc_status.velocity_body.raw.v = gps_ahrs_status.v;
	ngc_status.velocity_body.raw.u_sea.value = 0.0;
	ngc_status.velocity_body.raw.v_sea.value = 0.0;
	ngc_status.velocity_inertial.raw.xDot = gps_ahrs_status.northVel;
	ngc_status.velocity_inertial.raw.yDot = gps_ahrs_status.eastVel;
	ngc_status.velocity_inertial.raw.xDot_sea.value = 0.0;
	ngc_status.velocity_inertial.raw.yDot_sea.value = 0.0;
	ngc_status.velocity_inertial.raw.speed = gps_ahrs_status.speed;
	ngc_status.velocity_inertial.raw.course = gps_ahrs_status.course;


	status->ngc_status.set(ngc_status);
	*/
}


void GPS_AHRS::update_device_status(int r)
{
	GPS_AHRS_status gps_ahrs_status;
	gps_ahrs_status = gps_ahrs_access->get();

	if (r>0)
	{
		missed_receive_count=0;
		ahrs_status=DEVICE_RUNNING;
		gps_status = DEVICE_RUNNING;

		if ((gps_ahrs_status.llhPositionValidFlags & 0x0001)==0 || (gps_ahrs_status.nedVelocityValidFlags & 0x0001)==0) gps_status=DEVICE_WARNING;
	}

	else if (r <= 0)
	{
		//printf("FOG - ERROR reading message.\n");
		missed_receive_count++;
		if (missed_receive_count > GPS_AHRS_RECEIVE_WAIT_LOOPS_FOR_WARNING && missed_receive_count <= GPS_AHRS_RECEIVE_WAIT_LOOPS_FOR_FAULT)
		{
			ahrs_status = DEVICE_WARNING;
			gps_status = DEVICE_WARNING;
		}

		else if (missed_receive_count>GPS_AHRS_RECEIVE_WAIT_LOOPS_FOR_FAULT)
		{
			ahrs_status=DEVICE_FAULT;
			gps_status = DEVICE_FAULT;
			missed_receive_count=GPS_AHRS_RECEIVE_WAIT_LOOPS_FOR_FAULT;
		}
	}

	device_status = ahrs_status;

	gps_ahrs_status.ahrs_status = ahrs_status;
	gps_ahrs_status.gps_status = gps_status;

	gps_ahrs_access->set(gps_ahrs_status);
}


void GPS_AHRS::send_telemetry()
{
	GPS_AHRS_tlm_packet msg;

	GPS_AHRS_status gps_ahrs_status;
	gps_ahrs_status= gps_ahrs_access->get();

	gps_ahrs_status.compose_tlm_packet(msg);


	/*
	msg.roll=(int64)((is3dmgx335Data.roll)*AHRS_factor);
	msg.pitch=(int64)((is3dmgx335Data.pitch)*AHRS_factor);
	msg.heading=(int64)(mod180(is3dmgx335Data.yaw)*AHRS_factor);
	msg.xGyro=(int64)((is3dmgx335Data.xGyro)*AHRS_factor);
	msg.yGyro=(int64)((is3dmgx335Data.yGyro)*AHRS_factor);
	msg.zGyro=(int64)((is3dmgx335Data.zGyro)*AHRS_factor);
	msg.latitude=(int64)((is3dmgx335Data.latitude)*GPS_factor);
	msg.longitude=(int64)((is3dmgx335Data.longitude)*GPS_factor);
	msg.heightAboveEllipsode=(int64)((is3dmgx335Data.heightAboveEllipsode)*AHRS_factor);
	msg.heightAboveMSL=(int64)((is3dmgx335Data.heightAboveMSL)*AHRS_factor);
	msg.llhPositionValidFlags=(int64)((is3dmgx335Data.llhPositionValidFlags)*AHRS_factor);
	msg.north=(int64)((is3dmgx335Data.north)*AHRS_factor);
	msg.east=(int64)((is3dmgx335Data.east)*AHRS_factor);
	msg.down=(int64)((is3dmgx335Data.down)*AHRS_factor);
	msg.speed=(int64)((is3dmgx335Data.speed)*AHRS_factor);
	msg.groundSpeed=(int64)((is3dmgx335Data.groundSpeed)*AHRS_factor);
	msg.course=(int64)(mod180(is3dmgx335Data.heading)*AHRS_factor);
	msg.nedVelocityValidFlags=(int64)((is3dmgx335Data.nedVelocityValidFlags)*AHRS_factor);
	msg.xAccel=(int64)((is3dmgx335Data.xAccel)*AHRS_factor);
	msg.yAccel=(int64)((is3dmgx335Data.yAccel)*AHRS_factor);
	msg.zAccel=(int64)((is3dmgx335Data.zAccel)*AHRS_factor);
	msg.device_status=device_status;
*/

	tlmHMI->send_message((char*)&msg,sizeof(msg),true);
}


void* start_gps_ahrs(void *arg)
{
	GPS_AHRS* pThread=(GPS_AHRS*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}




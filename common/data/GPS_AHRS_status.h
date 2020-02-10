/*
 * GPS_AHRS_status.h
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */

#ifndef GPS_AHRS_STATUS_H_
#define GPS_AHRS_STATUS_H_

#include <math.h>
#include "../generic/custom_types.h"
#include "../generic/Variable.h"


struct GPS_AHRS_tlm_packet
{
	int64 roll;
	int64 pitch;
	int64 heading;
	int64 xGyro;
	int64 yGyro;
	int64 zGyro;
	int64 latitude;
	int64 longitude;
	int64 heightAboveEllipsoid;
	int64 heightAboveMSL;
	int64 northVel;
	int64 eastVel;
	int64 downVel;
	int64 speed;
	int64 groundSpeed;
	int64 course;
	int64 xAccel;
	int64 yAccel;
	int64 zAccel;
	int64 horizontalAccuracy;
	int64 verticalAccuracy;
	int64 speedAccuracy;
	int64 courseAccuracy;
	int64 nedVelocityValidFlags;
	int64 llhPositionValidFlags;
	int64 ahrs_status;
	int64 gps_status;
};

class GPS_AHRS_status
{
	private:

		void copy_data(const GPS_AHRS_status &d)
		{
			roll=d.roll;      pitch=d.pitch;    heading=d.heading;
			xGyro=d.xGyro;    yGyro=d.yGyro;    zGyro=d.zGyro;
			latitude=d.latitude;  longitude=d.longitude;
			heightAboveEllipsoid=d.heightAboveEllipsoid;  heightAboveMSL=d.heightAboveMSL;
			northVel =d.northVel;  eastVel =d.eastVel;  downVel =d.downVel;
			speed=d.speed;  groundSpeed=d.groundSpeed;  course=d.course;
			xAccel=d.xAccel;  yAccel=d.yAccel;  zAccel=d.zAccel;
			lat0 = d.lat0; lon0 = d.lon0; x0 = d.x0; y0 = d.y0; x = d.x; y = d.y; u = d.u; v = d.v;
			llhPositionValidFlags=d.llhPositionValidFlags;  nedVelocityValidFlags=d.nedVelocityValidFlags;
			ahrs_status=d.ahrs_status;
			gps_status = d.gps_status;
			timeStamp=d.timeStamp;

			compose_tlm_packet(tlm_packet);
		}


	public:

		Variable roll,pitch,heading;
		Variable xGyro,yGyro,zGyro;
		Variable latitude,longitude;
		Variable heightAboveEllipsoid,heightAboveMSL;
		Variable northVel,eastVel,downVel;
		Variable speed,groundSpeed,course;
		Variable xAccel,yAccel,zAccel;
		Variable lat0,lon0,x0,y0,x,y,u,v;
		Variable horizontalAccuracy,verticalAccuracy,speedAccuracy,courseAccuracy;
		int64 llhPositionValidFlags,nedVelocityValidFlags;
		int64 ahrs_status;
		int64 gps_status;
		int64 timeStamp;

		GPS_AHRS_tlm_packet tlm_packet;

		GPS_AHRS_status()
		{
			roll.zero();      pitch.zero();      heading.zero();
			xGyro.zero();     yGyro.zero();      zGyro.zero();
			latitude.zero();  longitude.zero();
			heightAboveEllipsoid.zero();  heightAboveMSL.zero();
			northVel.zero();  eastVel.zero();  downVel.zero();
			speed.zero();  groundSpeed.zero();  course.zero();
			xAccel.zero();  yAccel.zero();  zAccel.zero();
			lat0.zero(); lon0.zero(); x0.zero(); y0.zero(); x.zero(); y.zero(); u.zero(); v.zero();
			horizontalAccuracy.zero();  verticalAccuracy.zero();  speedAccuracy.zero();  courseAccuracy.zero();
			llhPositionValidFlags=0;  nedVelocityValidFlags=0;
			ahrs_status =0;
			gps_status = 0;
			timeStamp=0;


			tlm_packet.roll=0;      tlm_packet.pitch=0;      tlm_packet.heading=0;
			tlm_packet.xGyro=0;     tlm_packet.yGyro=0;      tlm_packet.zGyro=0;
			tlm_packet.latitude=0;  tlm_packet.longitude=0;
			tlm_packet.heightAboveEllipsoid=0;  tlm_packet.heightAboveMSL=0;
			tlm_packet.northVel =0;  tlm_packet.eastVel =0;  tlm_packet.downVel=0;
			tlm_packet.speed=0;  tlm_packet.groundSpeed=0;  tlm_packet.course=0;
			tlm_packet.xAccel=0;  tlm_packet.yAccel=0;  tlm_packet.zAccel=0;
			tlm_packet.horizontalAccuracy = 0;  tlm_packet.verticalAccuracy = 0;  tlm_packet.speedAccuracy = 0; tlm_packet.courseAccuracy = 0;
			tlm_packet.llhPositionValidFlags=0;  tlm_packet.nedVelocityValidFlags=0;
			tlm_packet.ahrs_status =0;
			tlm_packet.gps_status = 0;
		}


		GPS_AHRS_status(GPS_AHRS_status &d)
		{
			copy_data(d);
		}


		~GPS_AHRS_status(){}


		GPS_AHRS_status& operator=(const GPS_AHRS_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(GPS_AHRS_tlm_packet &tp)
		{
			tp.roll=(int64)(roll.value*(180.0/M_PI)*AHRS_factor);
			tp.pitch=(int64)(pitch.value*(180.0/M_PI)*AHRS_factor);
			tp.heading=(int64)(heading.value*(180.0/M_PI)*AHRS_factor);
			tp.xGyro=(int64)(xGyro.value*(180.0/M_PI)*AHRS_factor);
			tp.yGyro=(int64)(yGyro.value*(180.0/M_PI)*AHRS_factor);
			tp.zGyro=(int64)(zGyro.value*(180.0/M_PI)*AHRS_factor);
			tp.latitude=(int64)(latitude.value*GPS_factor);
			tp.longitude=(int64)(longitude.value*GPS_factor);
			tp.heightAboveEllipsoid=(int64)(heightAboveEllipsoid.value*AHRS_factor);
			tp.heightAboveMSL=(int64)(heightAboveMSL.value*AHRS_factor);
			tp.northVel =(int64)(northVel.value*AHRS_factor);
			tp.eastVel =(int64)(eastVel.value*AHRS_factor);
			tp.downVel =(int64)(downVel.value*AHRS_factor);
			tp.speed=(int64)(speed.value*AHRS_factor);
			tp.groundSpeed=(int64)(groundSpeed.value*AHRS_factor);
			tp.course=(int64)(course.value*AHRS_factor);
			tp.xAccel=(int64)(xAccel.value*AHRS_factor);
			tp.yAccel=(int64)(yAccel.value*AHRS_factor);
			tp.zAccel=(int64)(zAccel.value*AHRS_factor);
			tp.horizontalAccuracy = (int64)(horizontalAccuracy.value * AHRS_factor);
			tp.verticalAccuracy = (int64)(verticalAccuracy.value * AHRS_factor);
			tp.speedAccuracy = (int64)(speedAccuracy.value * AHRS_factor);
			tp.courseAccuracy = (int64)(courseAccuracy.value * AHRS_factor);
			tp.llhPositionValidFlags=(int64)(llhPositionValidFlags);
			tp.nedVelocityValidFlags=(int64)(nedVelocityValidFlags);
			tp.ahrs_status=(int64)(ahrs_status);
			tp.gps_status = (int64)(gps_status);
		}


		void extract_tlm_packet(GPS_AHRS_tlm_packet tp)
		{
			roll.value = (((double)(tp.roll)) / AHRS_factor) * M_PI / 180.0;
			pitch.value = (((double)(tp.pitch)) / AHRS_factor) * M_PI / 180.0;
			heading.value = (((double)(tp.heading)) / AHRS_factor) * M_PI / 180.0;

			xGyro.value = (((double)(tp.xGyro)) / AHRS_factor) * M_PI / 180.0;
			yGyro.value = (((double)(tp.yGyro)) / AHRS_factor) * M_PI / 180.0;
			zGyro.value = (((double)(tp.zGyro)) / AHRS_factor) * M_PI / 180.0;

			latitude.value = (((double)(tp.latitude)) / GPS_factor) ;
			longitude.value = (((double)(tp.longitude)) / GPS_factor) ;

			heightAboveEllipsoid.value = (((double)(tp.heightAboveEllipsoid)) / AHRS_factor);
			heightAboveMSL.value = (((double)(tp.heightAboveMSL)) / AHRS_factor);
			
			northVel.value = (((double)(tp.northVel)) / AHRS_factor);
			eastVel.value = (((double)(tp.eastVel)) / AHRS_factor);
			downVel.value = (((double)(tp.downVel)) / AHRS_factor);
			speed.value = (((double)(tp.speed)) / AHRS_factor);
			groundSpeed.value = (((double)(tp.groundSpeed)) / AHRS_factor);
			course.value = (((double)(tp.course)) / AHRS_factor);
		
			xAccel.value = (((double)(tp.xAccel)) / AHRS_factor);
			yAccel.value = (((double)(tp.yAccel)) / AHRS_factor);
			zAccel.value = (((double)(tp.zAccel)) / AHRS_factor);
			
			horizontalAccuracy.value = (((double)(tp.horizontalAccuracy)) / AHRS_factor);
			verticalAccuracy.value = (((double)(tp.verticalAccuracy)) / AHRS_factor);
			speedAccuracy.value = (((double)(tp.speedAccuracy)) / AHRS_factor);
			courseAccuracy.value = (((double)(tp.courseAccuracy)) / AHRS_factor);

			llhPositionValidFlags = tp.llhPositionValidFlags;
			nedVelocityValidFlags = tp.nedVelocityValidFlags;
			ahrs_status = tp.ahrs_status;
			gps_status = tp.gps_status;
			
		}
};


#endif /* GPS_AHRS_STATUS_H_ */

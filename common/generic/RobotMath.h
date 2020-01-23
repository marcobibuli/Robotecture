/*
 * RobotMath.h
 *
 *  Created on: Jun 19, 2017
 *      Author: mx
 */

#ifndef COMMON_ROBOTMATH_H_
#define COMMON_ROBOTMATH_H_

#include <math.h>

double modpi(double a);
double mod180(double a);
void compute_xy_from_lat_lon(double Lat,double Lon,double &x,double &y,double &utmzone,char &utmzone_char);
void compute_lat_lon_from_xy(double x,double y,double utmzone,char utmzone_char,double &Lat,double &Lon);
double sat(double x,double min, double max);

double pythag(double a,double b);

double DMAX(double a,double b);
double DMIN(double a,double b);
double max_vect(double *v,int n);
double min_vect(double *v,int n);
int IMIN(int a,int b);
double SQR(double a);

void xRot(double ang,double xi,double yi,double zi,double &xo,double &yo,double &zo);
void yRot(double ang,double xi,double yi,double zi,double &xo,double &yo,double &zo);
void zRot(double ang,double xi,double yi,double zi,double &xo,double &yo,double &zo);



#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define sign(a) ((a) >= 0.0 ? 1.0 : -1.0)

#endif /* COMMON_ROBOTMATH_H_ */

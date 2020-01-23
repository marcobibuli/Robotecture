/*
 * RobotMath.cpp
 *
 *  Created on: Jun 19, 2017
 *      Author: mx
 */


#include "RobotMath.h"


double modpi(double x)
{
	double y=x;

	while (y>M_PI)
		y=y-2.0*M_PI;

	while (y<=-M_PI)
	    y=y+2.0*M_PI;

	return y;
}


double mod180(double x)
{
	double y=x;

	while (y>180.0)
		y=y-360.0;

	while (y<=-180.0)
	    y=y+360.0;

	return y;
}


void compute_lat_lon_from_xy(double x,double y,double utmzone,char utmzone_char,double &Lat,double &Lon)
{
	double t=x;
	x=y;
	y=t;
	char hemis;
	if (utmzone_char>'M') hemis='N';
	else hemis='S';

	double sa = 6378137.000000;
	double sb = 6356752.314245;

	double e2 = pow( ( pow( sa , 2.0 ) - pow( sb , 2.0 ) ) , 0.5 ) / sb;
	double e2cuadrada = pow(e2 , 2.0);
	double c = pow( sa , 2.0 ) / sb;

	double X = x-500000.0;
	double Y=y;
	if (hemis == 'S') Y=y-10000000.0;

	double S = ( ( utmzone * 6.0 ) - 183.0 );
	double lat =  Y / ( 6366197.724 * 0.9996 );
	double v= (c/pow(1.0 + e2cuadrada * pow(cos(lat),2.0) , 0.5))*0.9996;
	double a = X / v;
	double a1 = sin( 2.0 * lat );
	double a2 = a1 * pow( cos(lat)  , 2.0);
	double j2 = lat + ( a1 / 2.0 );
	double j4 = ( ( 3.0 * j2 ) + a2 ) / 4.0;
	double j6 = ( ( 5.0 * j4 ) + ( a2 * pow( cos(lat) , 2.0)) ) / 3.0;
	double alfa = ( 3.0 / 4.0 ) * e2cuadrada;
	double beta = ( 5.0 / 3.0 ) * pow(alfa , 2.0);
	double gama = ( 35.0 / 27.0 ) * pow(alfa , 3.0);
	double Bm = 0.9996 * c * ( lat - alfa * j2 + beta * j4 - gama * j6 );
	double b = ( Y - Bm ) / v;
	double Epsi = ( ( e2cuadrada * pow(a , 2.0) ) / 2.0 ) * pow(cos(lat) , 2.0);
	double Eps = a * ( 1.0 - ( Epsi / 3.0 ) );
	double nab = ( b * ( 1.0 - Epsi ) ) + lat;
	double senoheps = ( exp(Eps) - exp(-Eps) ) / 2.0;
	double Delt = atan(senoheps / (cos(nab) ) );
	double TaO = atan(cos(Delt) * tan(nab));
	Lon = (Delt *(180.0 / M_PI ) ) + S;
	Lat = ( lat + ( 1.0 + e2cuadrada* pow(cos(lat), 2.0) - ( 3.0 / 2.0 ) * e2cuadrada * sin(lat) * cos(lat) * ( TaO - lat ) ) * ( TaO - lat ) ) * (180.0 / M_PI);
}

void compute_xy_from_lat_lon(double Lat,double Lon,double &x,double &y,double &utmzone,char &utmzone_char)
{
	double lat=Lat*M_PI/180.0;
	double lon=Lon*M_PI/180.0;

	double a = 6378137;        // semi-major axis
	double b = 6356752.314245; // semi-minor axis
	double e = sqrt(1.0-pow(b/a,2.0)); // eccentricity

	double Lon0 = floor(Lon/6.0)*6.0+3.0; // reference longitude in degrees
	double lon0 = Lon0*M_PI/180.0;      // in radians
	double k0 = 0.9996;               // scale on central meridian

	double FE = 500000.0;              // false easting
	double FN = (Lat < 0.0)*10000000.0; // false northing

	double eps = pow(e,2.0)/(1.0-pow(e,2.0));  // e prime square

	double N = a/sqrt(1-pow(e,2.0)*pow(sin(lat),2.0));
	double T = pow(tan(lat),2.0);
	double C = (pow(e,2.0)/(1.0-pow(e,2.0)))*pow((cos(lat)),2.0);
	double A = (lon-lon0)*cos(lat);

	double M = a*(  ( 1.0 - pow(e,2.0)/4.0 - 3.0*pow(e,4.0)/64.0 - 5.0*pow(e,6.0)/256.0 ) * lat
			   -( 3.0*pow(e,2.0)/8.0 + 3.0*pow(e,4.0)/32.0 + 45.0*pow(e,6.0)/1024.0 ) * sin(2.0*lat)
			   +( 15.0*pow(e,4.0)/256.0 + 45.0*pow(e,6.0)/1024.0 ) * sin(4.0*lat)
			   -(35.0*pow(e,6.0)/3072.0 ) * sin(6.0*lat) );

	y = FE + k0*N*( A + (1-T+C) * pow(A,3.0)/6.0 + (5.0-18.0*T+pow(T,2.0) + 72.0*C-58.0*eps) * pow(A,5.0)/120.0 );

	x = FN + k0*M + k0*N*tan(lat)*( pow(A,2.0)/2.0 + (5.0 - T + 9.0*C + 4.0*pow(C,2.0)) * pow(A,4.0)/24.0
		+ (61.0 - 58.0*T + pow(T,2.0) + 600.0*C - 330.0*eps) * pow(A,6.0)/720.0 );

	utmzone = floor(Lon0/6.0)+31.0;

	if (Lat<-80.0) utmzone_char='B';
	else if (Lat>=-80.0 && Lat<-72.0) utmzone_char='C';
	else if (Lat>=-72.0 && Lat<-64.0) utmzone_char='D';
	else if (Lat>=-64.0 && Lat<-56.0) utmzone_char='E';
	else if (Lat>=-56.0 && Lat<-48.0) utmzone_char='F';
	else if (Lat>=-48.0 && Lat<-40.0) utmzone_char='G';
	else if (Lat>=-40.0 && Lat<-32.0) utmzone_char='H';
	else if (Lat>=-32.0 && Lat<-24.0) utmzone_char='J';
	else if (Lat>=-24.0 && Lat<-16.0) utmzone_char='K';
	else if (Lat>=-16.0 && Lat<-8.0) utmzone_char='L';
	else if (Lat>=-8.0 && Lat<0.0) utmzone_char='M';
	else if (Lat>=0.0 && Lat<8.0) utmzone_char='N';
	else if (Lat>=8.0 && Lat<16.0) utmzone_char='P';
	else if (Lat>=16.0 && Lat<24.0) utmzone_char='Q';
	else if (Lat>=24.0 && Lat<32.0) utmzone_char='R';
	else if (Lat>=32.0 && Lat<40.0) utmzone_char='S';
	else if (Lat>=40.0 && Lat<48.0) utmzone_char='T';
	else if (Lat>=48.0 && Lat<56.0) utmzone_char='U';
	else if (Lat>=56.0 && Lat<64.0) utmzone_char='V';
	else if (Lat>=64.0 && Lat<72.0) utmzone_char='W';
	else if (Lat>=72.0 && Lat<84.0) utmzone_char='X';
	else if (Lat>=84.0) utmzone_char='Y';
}


double sat(double x,double min, double max)
{
	double y=x;
	if (x>max) y=max;
	if (x<min) y=min;
	return y;
}


double pythag(double a,double b)
// computes sqrt(a^2+b^2) without destructive underflow or overflow
{
	double absa,absb;

	absa=fabs(a);
	absb=fabs(b);
	if(absa>absb) return absa*sqrt(1.0+SQR(absb/absa));
	else return (absb==0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}

double DMAX(double a,double b)
{
	double dmaxarg1,dmaxarg2;
	return (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ? (dmaxarg1) : (dmaxarg2));
}

double DMIN(double a,double b)
{
	double dmaxarg1,dmaxarg2;
	return (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) < (dmaxarg2) ? (dmaxarg1) : (dmaxarg2));
}

int IMIN(int a,int b)
{
	int iminarg1,iminarg2;
	return (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ? (iminarg1) : (iminarg2));
}


double SQR(double a)
{
	 double sqrarg;
	 return ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg);
}


void xRot(double ang,double xi,double yi,double zi,double &xo,double &yo,double &zo)
{
	xo=xi*1.0 + yi*0.0      + zi*0.0;
	yo=xi*0.0 + yi*cos(ang) - zi*sin(ang);
	zo=xi*0.0 + yi*sin(ang) + zi*cos(ang);
}

void yRot(double ang,double xi,double yi,double zi,double &xo,double &yo,double &zo)
{
	xo= xi*cos(ang) + yi*0.0 + zi*sin(ang);
	yo= xi*0.0      + yi*1.0 + zi*0.0;
	zo=-xi*sin(ang) + yi*0.0 + zi*cos(ang);
}

void zRot(double ang,double xi,double yi,double zi,double &xo,double &yo,double &zo)
{
	xo= xi*cos(ang) - yi*sin(ang) + zi*0.0;
	yo= xi*sin(ang) + yi*cos(ang) + zi*0.0;
	zo= xi*0.0      + yi*0.0      + zi*1.0;
}

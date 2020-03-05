// Mx Robotic Framework
// Coded by Marco Bibuli
// V1.0 - 28/07/2016


#ifndef __CUSTOM_TYPES__
#define __CUSTOM_TYPES__

typedef signed char  int8;
typedef signed short int16;
typedef signed long  int32;
typedef signed long long int64;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;
typedef unsigned long long uint64;

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;


// double <-> int convertion factors

#define IO_factor (100000.0)
#define Motors_factor (100000.0)
#define FOG_factor (100000.0)
#define GPS_factor (100000000.0)
#define AHRS_factor (100000.0)
#define CTD_factor (100000.0)
#define DVL_factor (100000.0)
#define PA500_factor (100000.0)
#define Echologger_factor (100000.0)
#define NGC_factor (100000.0)
#define Task_factor (100000.0)
#define USBLpos_factor (100000.0)





#endif

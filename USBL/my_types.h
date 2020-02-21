/*=========================================================================*\
||                                                                         ||
||                   Code written by: Gabriele Bruzzone                    ||
||                                                                         ||
\*=========================================================================*/

// Release 1.1, 16/03/2004, added check on sizes
// Release 1.0, 08/03/2000, written and tested

//---------------------------------------------------------------------------
//
// my_types.h
//
//---------------------------------------------------------------------------

#ifndef MY_TYPES_H
#define MY_TYPES_H

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

typedef signed char  int8;
typedef signed short int16;
typedef signed long  int32;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

#ifdef __MAIN__
class _MyTypesHClass
   {
   public:
      _MyTypesHClass()
         {
         if(sizeof(int8)!=1)
            {
            dprintf_1("int8 has a wrong size: %lu, please check my_types.h file\n",
                   sizeof(int8));
            exit(1);
            }
         if(sizeof(int16)!=2)
            {
            dprintf_1("int16 has a wrong size: %lu, please check my_types.h file\n",
                   sizeof(int16));
            exit(1);
            }
         if(sizeof(int32)!=4 && sizeof(int32)!=8 )      /// AR: allow to run also on x86_64 architectures
            {
            dprintf_1("int32 has a wrong size: %lu, please check my_types.h file\n",
                   sizeof(int32));
            exit(1);
            }
         dprintf("Checked types sizes: OK\n");
         };
   };
_MyTypesHClass _myTypesHClass;
#endif // __MAIN__

#ifdef __CYGWIN__
   #include "stub.h"
#endif

#endif // MY_TYPES_H

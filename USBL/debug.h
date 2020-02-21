/*=========================================================================*\
||                                                                         ||
||                  Code written by: Gabriele Bruzzone                     ||
||                                                                         ||
\*=========================================================================*/

// Release 1.0, 18/04/2004

//---------------------------------------------------------------------------
//
// debug.h
//
//---------------------------------------------------------------------------

#ifndef DEBUG_H
#define DEBUG_H

// comment the following line if you don't wish logging debug info on file
//#define __DEBUG_ON_FILE__

#ifdef __MAIN__
   #define PRE
#else
   #define PRE extern
#endif // __MAIN__

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef __DEBUG_ON_FILE__

PRE FILE* _debugFp;

#ifdef __MAIN__
const char* _debugFileName="errors.txt";
class _DebugHClass
   {
   public:
      _DebugHClass()
         {
         if((_debugFp=fopen(_debugFileName,"w"))==NULL)
            {
            printf("Can not open %s file\n",_debugFileName);
            exit(1);
            }
         printf("Opened debugging file: %s\n",_debugFileName);
         };
      ~_DebugHClass()
         {
         fclose(_debugFp);
         printf("Closed debugging file: %s\n",_debugFileName);
         }
   };

_DebugHClass _debugHClass;
#endif // __MAIN__

#endif // __DEBUG_ON_FILE__


#define require(x) assert(x)

#ifdef __DEBUG_ON_FILE__
#define log_errno(r,e) \
        if(r!=0)\
           {\
           fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,strerror(e));\
           fflush(stdout);\
           fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,strerror(e));\
           fflush(_debugFp);\
           }

#define log_error(r) \
        if(r!=0)\
        {\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,strerror(r));\
        fflush(stdout);\
        fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,strerror(r));\
        fflush(_debugFp);\
        }
        

#define dprintf(x0)\
        {sprintf(_debugString,x0);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);\
        fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(_debugFp);}

#define dprintf_1(x0,x1)\
        {sprintf(_debugString,x0,x1);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);\
        fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(_debugFp);}

#define dprintf_2(x0,x1,x2)\
        {sprintf(_debugString,x0,x1,x2);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);\
        fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(_debugFp);}

#define dprintf_3(x0,x1,x2,x3)\
        {sprintf(_debugString,x0,x1,x2,x3);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);\
        fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(_debugFp);}

#define dprintf_4(x0,x1,x2,x3,x4)\
        {sprintf(_debugString,x0,x1,x2,x3,x4);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);\
        fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(_debugFp);}

#define dprintf_5(x0,x1,x2,x3,x4,x5)\
        {sprintf(_debugString,x0,x1,x2,x3,x4,x5);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);\
        fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(_debugFp);}

#define dprintf_6(x0,x1,x2,x3,x4,x5,x6)\
        {sprintf(_debugString,x0,x1,x2,x3,x4,x5,x6);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);\
        fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(_debugFp);}

#define dprintf_7(x0,x1,x2,x3,x4,x5,x6,x7)\
        {sprintf(_debugString,x0,x1,x2,x3,x4,x5,x6,x7);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);\
        fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(_debugFp);}

#define dprintf_8(x0,x1,x2,x3,x4,x5,x6,x7,x8)\
        {sprintf(_debugString,x0,x1,x2,x3,x4,x5,x6,x7,x8);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);\
        fprintf(_debugFp,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(_debugFp);}
#else
#define log_errno(r,e) \
        if(r!=0)\
           {\
           fprintf(stdout,"%s:%d - %s\n",__FILE__,__LINE__,strerror(e));\
           fflush(stdout);\
           }

#define log_error(r) \
        if(r!=0)\
        {fprintf(stdout,"%s:%d - %s\n",__FILE__,__LINE__,strerror(r));\
        fflush(stdout);}

#define dprintf(x0)\
        {sprintf(_debugString,x0);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);}

#define dprintf_1(x0,x1)\
        {sprintf(_debugString,x0,x1);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);}

#define dprintf_2(x0,x1,x2)\
        {sprintf(_debugString,x0,x1,x2);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);}

#define dprintf_3(x0,x1,x2,x3)\
        {sprintf(_debugString,x0,x1,x2,x3);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);}

#define dprintf_4(x0,x1,x2,x3,x4)\
        {sprintf(_debugString,x0,x1,x2,x3,x4);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);}

#define dprintf_5(x0,x1,x2,x3,x4,x5)\
        {sprintf(_debugString,x0,x1,x2,x3,x4,x5);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);}

#define dprintf_6(x0,x1,x2,x3,x4,x5,x6)\
        {sprintf(_debugString,x0,x1,x2,x3,x4,x5,x6);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);}

#define dprintf_7(x0,x1,x2,x3,x4,x5,x6,x7)\
        {sprintf(_debugString,x0,x1,x2,x3,x4,x5,x6,x7);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);}

#define dprintf_8(x0,x1,x2,x3,x4,x5,x6,x7,x8)\
        {sprintf(_debugString,x0,x1,x2,x3,x4,x5,x6,x7,x8);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);}
#endif // __DEBUG_ON_FILE__

#endif // DEBUG_H

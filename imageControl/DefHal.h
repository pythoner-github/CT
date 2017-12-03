#ifndef __DEFHAL_H__
#define __DEFHAL_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>
#include <Def.h>

//typedef
typedef int INT32;
typedef unsigned int INT32U;
typedef short INT16;
typedef unsigned short INT16U;
typedef char INT8;
typedef unsigned char INT8U;

const int BITS8=1;
const int BITS16=2;
const int BITS32=4;

//error info
const int SUCCESS=1; // sucess when exit
const int ERR_OP=0;// open or operation error
const int ERR_RANGE=-2;// input if out of range
const int ERR_MALLOC=-3;// malloc or map error
const int ERR_SYSCALL=-4;// system call error	

#endif

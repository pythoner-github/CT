
#ifndef __CGOSGPIO_H__
#define __CGOSGPIO_H__

#include "Cgos.h"

class CgosGpioI2C
{
public:
    CgosGpioI2C();
    ~CgosGpioI2C();

    int I2CWrite(unsigned char addr, int len, unsigned char buf[]);
    int I2CRead(unsigned char addr, unsigned char reg,int len, unsigned char buf[]);

private:
    HCGOS hCgos;  

};



#endif

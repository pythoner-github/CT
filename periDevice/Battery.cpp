#include <stdio.h>
#include "Battery.h"
#include "I2CCtrl.h"
#include "CgosGpio.h"
#include "Def.h"
unsigned char Battery::ADDR=0x0b;
unsigned char Battery::GET_CHARGE=0x0d;
int Battery::GetCapacity(void)
{
    int ret;
    unsigned char buffer = 0;
    I2CInterface i2c;
    CgosGpioI2C cgosI2C;
    ret = i2c.I2CRead( ADDR, GET_CHARGE, 1, &buffer);
    if (ret < 0)
    {
        if ( cgosI2C.I2CRead(ADDR, GET_CHARGE, 1, &buffer) )
        {
            return buffer;
        }
        PRINTF("read Capacity fail\n");
        return 0;
    }
    return buffer;
}


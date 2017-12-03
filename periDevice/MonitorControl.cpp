#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <iostream>
#include "MonitorControl.h"
#include "I2CCtrl.h"
#include "CgosGpio.h"

using namespace std;

MonitorControl::MonitorControl()
{

}

MonitorControl::~MonitorControl()
{

}

int MonitorControl::SetBrightAdd(void)
{
    DisplayWrite1( BRIGHT_ADD );
    DisplayWrite( SET_BRIGHT_ADD );
    return 0;
}

int MonitorControl::SetBrightSub(void)
{
    DisplayWrite1( BRIGHT_SUB );
    DisplayWrite( SET_BRIGHT_SUB );
    return 0;
}

int MonitorControl::SetContrastAdd(void)
{
    DisplayWrite1( CONTRAST_ADD );
    DisplayWrite( SET_CONTRAST_ADD );
    return 0;
}

int MonitorControl::SetContrastSub(void)
{
    DisplayWrite1( CONTRAST_SUB );
    DisplayWrite( SET_CONTRAST_SUB );
    return 0;
}

int MonitorControl::PowerOff(void)
{
    DisplayWrite1( POWER_OFF );
    DisplayWrite( SET_POWER_OFF );
    return 0;
}

int MonitorControl::PowerOn(void)
{
    DisplayWrite1( POWER_ON );
    DisplayWrite( SET_POWER_ON );
    return 0;
}

int MonitorControl::AutoAdjust(void)
{
    DisplayWrite1( AUTO_ADJUST );
    DisplayWrite( SET_AUTO_ADJUST );
    return 0;
}

int MonitorControl::AutoBalance(void)
{
    DisplayWrite1( AUTO_BALANCE ) ;
    DisplayWrite( SET_AUTO_BALANCE ) ;
    return 0;
}

//巨峰///////////
int MonitorControl::DisplayWrite(unsigned char reg, unsigned char value)
{
    unsigned char buffer[2] = { reg, value };
    I2CInterface i2c;
    CgosGpioI2C cgosI2C;

    if (value == INVALID_VALUE)
    {
        cgosI2C.I2CWrite(VIDEO_ADDR2, 1, buffer);

        return i2c.I2CWrite(VIDEO_ADDR2, 1, buffer);
    }
    else
    {

        cgosI2C.I2CWrite(VIDEO_ADDR2, 2, buffer);

        return i2c.I2CWrite(VIDEO_ADDR2, 2, buffer);
    }
}

///睿显//////////
int MonitorControl::DisplayWrite1(unsigned char reg)
{
    unsigned char buffer[2] = { reg,reg };
    I2CInterface i2c;
    CgosGpioI2C cgosI2C;

    cgosI2C.I2CWrite(VIDEO_ADDR1, 1, buffer);

    return i2c.I2CWrite(VIDEO_ADDR1, 1, buffer);
}


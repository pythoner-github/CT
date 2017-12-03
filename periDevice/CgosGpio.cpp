
#include <stdio.h>
#include "CgosGpio.h"

#undef TEXT

#ifndef TEXT
#define TEXT(s) s
#endif


void report(char *s)
{
    //__report("%s",s);
    //printf("%s",s);
}

CgosGpioI2C::CgosGpioI2C()
{
    hCgos = 0;

    // install the library
    if (!CgosLibInitialize())
    {
        if (!CgosLibInstall(1))
        {
            // error: can't install cgos library
            report(TEXT("Error: can't install CGOS library\n"));
        }
        report(TEXT("The driver has been installed.\n"));
        if (!CgosLibInitialize()) {
            report(TEXT("Still could not open driver, a reboot might be required!\n"));
        }		
    }

    // open the cgos board
    if (CgosBoardOpen(0,0,0,&hCgos))
    {
        report(TEXT("open CGOS board\n"));
    } else
    {
        // error: can't open board
        report(TEXT("Error: can't open CGOS board\n"));
    }
    CgosI2CSetFrequency( hCgos,0,100000);

}

CgosGpioI2C::~CgosGpioI2C()
{

    if (hCgos) CgosBoardClose(hCgos);
    
    CgosLibUninitialize();
}

int CgosGpioI2C::I2CWrite(unsigned char addr, int len, unsigned char buf[])
{
    if( CgosI2CWrite(hCgos, 0, addr<<1,buf, len) )
    {
        return 1 ;
    }
    else
    {
        report(TEXT("CgosI2CWrite fail\n"));
        return 0;
    }

}


int CgosGpioI2C::I2CRead(unsigned char addr, unsigned char reg,int len, unsigned char buf[])
{
    unsigned char tmp_reg = reg;

    if (CgosI2CWriteReadCombined(hCgos,0, (addr<<1), &tmp_reg, 1, buf, 1))
    {
        report(TEXT("CgosI2CWriteRead ok \n"));
        return 1;
    }

    else
    {
        //report(TEXT("CgosI2CWrite fail\n"));
        report(TEXT("CgosI2CReadRegister fail\n"));
        return 0;
    }

}

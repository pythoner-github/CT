#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "Def.h"
#include "I2CCtrl.h"

I2CInterface::I2CInterface()
{
    m_fd = open("/dev/i2c-1",O_RDWR);
    if (m_fd < 0){
        PRINTF("open i2c device fail\n");
    }
}

I2CInterface::~I2CInterface()
{
    if ( m_fd >= 0 )
        close(m_fd);
}

int I2CInterface::I2CWrite( unsigned char addr, int len ,unsigned char buf[])
{
    int ret;
    struct i2c_rdwr_ioctl_data ioctl_data;

    ioctl_data.nmsgs = 1 ;
    ioctl_data.msgs = (struct i2c_msg *)malloc(ioctl_data.nmsgs * sizeof(ioctl_data.msgs));
    if (ioctl_data.msgs == NULL){
        PRINTF("malloc mem fail\n");
        return -1;;
    }
    ioctl_data.msgs[0].addr = addr;
    ioctl_data.msgs[0].flags = 0;//read_write
    ioctl_data.msgs[0].len = len + 1;
    ioctl_data.msgs[0].buf = buf;

    ret = ioctl(m_fd, I2C_RDWR, (unsigned long)&ioctl_data); 

    //free(ioctl_data.msgs);

    return ret;
}

int I2CInterface::I2CRead( unsigned char addr, unsigned char reg, int len, unsigned char buffer[])
{
    int ret;
    struct i2c_rdwr_ioctl_data ioctl_data;
    struct i2c_msg msgs[2];
    unsigned char tmp_reg = reg;

    ioctl_data.nmsgs = 2 ;
    ioctl_data.msgs = msgs;
    ioctl_data.msgs[0].addr = addr;
    ioctl_data.msgs[0].flags = 0;//read_write
    ioctl_data.msgs[0].len = 1;
    ioctl_data.msgs[0].buf = &tmp_reg; 
    //ioctl_data.msgs[0].buf = (unsigned char*)malloc(1);
    //ioctl_data.msgs[0].buf[0] = reg;

    ioctl_data.msgs[1].addr = addr;
    ioctl_data.msgs[1].flags = I2C_M_RD;//read_write
    ioctl_data.msgs[1].len = 1;
    ioctl_data.msgs[1].buf = buffer;

    ret = ioctl(m_fd, I2C_RDWR, (unsigned long)&ioctl_data); 

    //free(ioctl_data.msgs[0].buf);
    //free(ioctl_data.msgs);

    return ret;
}

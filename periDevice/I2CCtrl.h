#ifndef _I2CInterface__H_
#define _I2CInterface__H_

class I2CInterface
{
    public:
        I2CInterface();
        ~I2CInterface();

        int I2CWrite( unsigned char addr, int len, unsigned char buf[]);
        int I2CRead( unsigned char addr, unsigned char reg, int len, unsigned char buf[]);
    private:

        int m_fd;
};

#endif
